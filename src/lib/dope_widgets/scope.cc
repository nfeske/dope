/*
 * \brief   DOpE Scope widget module
 * \date    2002-05-15
 * \author  Norman Feske
 *
 * A Scope is a namespace.
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

struct scope;
#define WIDGET struct scope

#include "dopestd.h"
#include "hashtab.h"
#include "scope.h"
#include "script.h"
#include "widman.h"
#include "appman.h"
#include "widget_data.h"
#include "widget_help.h"

enum {
	VAR_HASHTAB_SIZE = 32,   /* applications variable hash table config */
	VAR_HASH_CHARS   =  5,
};

static struct hashtab_services *hashtab;
static struct widman_services  *widman;
static struct script_services  *script;
static struct appman_services  *appman;

struct scope_data {
	HASHTAB *vars;
};

struct variable {
	char const *name;   /* variable name  */
	char const *type;   /* variable type  */
	WIDGET     *value;  /* variable value */
};

int init_scope(struct dope_services *d);


/****************************
 ** General widget methods **
 ****************************/

/**
 * Deallocate scope data
 */
static void scope_free_data(SCOPE *s)
{
	struct variable *var;
	WIDGET *w;

	/* delete variables and dissolve the references to their widgets */
	var = (variable *)(hashtab->get_first(s->sd->vars));
	while (var) {
		if ((w = var->value))
			w->gen->dec_ref(w);
		var = (variable *)(hashtab->get_next(s->sd->vars, var));
	}

	/* now, destroy the hash table */
	hashtab->dec_ref(s->sd->vars);
}


/**
 * Return widget type identifier
 */
static char const *scope_get_type(SCOPE *v)
{
	return "Scope";
}


/****************************
 ** Scope specific methods **
 ****************************/

/**
 * Redefine a variable or create a new one if needed
 *
 * The type argument is not replicated. The caller must ensure that the
 * type string exists as long as the variable exists!
 */
static int scope_set_var(SCOPE *s, char const *type, char const *name, size_t len, WIDGET *value)
{
	struct variable *v;

	/* does variable already exists? */
	v = (variable *)(hashtab->get_elem(s->sd->vars, name, len));
	
	/* create a new variable */
	if (!v) {
		int name_len = MIN(strlen(name), len);
		v = (variable *)(zalloc(sizeof(struct variable) + strlen(name) + 2));
		if (!v) return -1;
		char *buf = (char *)((adr)v + sizeof(struct variable));
		memcpy(buf, name, name_len);
		buf[name_len] = 0;
		v->name = buf;
		INFO(printf("scope_set_var: variable %s\n", v->name));
		hashtab->add_elem(s->sd->vars, v->name, v);
	} else {

		/* loose the reference to the old content */
		if (v->value)
			v->value->gen->dec_ref(v->value);
	}
	v->type  = type;
	v->value = value;
	return 0;
}


/**
 * Request the value of a variable
 */
static WIDGET *scope_get_var(SCOPE *s, char const *name, int len)
{
	struct variable *v = (variable *)(hashtab->get_elem(s->sd->vars, name, len));
	return v ? v->value : NULL;
}


/**
 * Request the type of a variable
 */
static char const *scope_get_vartype(SCOPE *s, char const *name, int len)
{
	struct variable *v = (variable *)(hashtab->get_elem(s->sd->vars, name, len));
	return v ? v->type : NULL;
}


/**
 * Request subscope with the specified name
 */
static SCOPE *scope_get_subscope(SCOPE *s, char const *name, int len)
{
	struct variable *v = (variable *)(hashtab->get_elem(s->sd->vars, name, len));

	if (!v || !v->type || !v->value) return NULL;
	if (!streq(v->type, "Scope", 256)) return NULL;

	return v->value;
}


/**
 * Import scope of another application
 */
static int scope_voodoo(SCOPE *s, char *app_name)
{
	SCOPE *rs;
	int app_id;
	
	/* resolve application root scope to rip off */
	app_id = appman->app_id_of_name(app_name);
	if (app_id < 0) return 0;
	rs = appman->get_rootscope(app_id);
	if (!rs) return 0;

	/* make scope use the same hashtab as the requested application */
	hashtab->dec_ref(s->sd->vars);
	hashtab->inc_ref(rs->sd->vars);
	s->sd->vars = rs->sd->vars;

	return 1;
}


static struct widget_methods gen_methods;
static struct scope_methods scope_methods = {
	scope_set_var,
	scope_get_var,
	scope_get_vartype,
	scope_get_subscope,
};


/***********************
 ** Service functions **
 ***********************/

static SCOPE *create(void)
{
	SCOPE *scope = ALLOC_WIDGET(struct scope);
	SET_WIDGET_DEFAULTS(scope, struct scope, &scope_methods);

	/* create hash table to store the variables of the scope */
	scope->sd->vars = hashtab->create(VAR_HASHTAB_SIZE, VAR_HASH_CHARS);
	if (!scope->sd->vars) {
		free(scope);
		return NULL;
	}
	return scope;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct scope_services services = {
	create
};


/************************
 ** Module entry point **
 ************************/

static void build_script_lang(void)
{
	widtype *widtype = script->reg_widget_type("Scope", (void *(*)(void))create);
	script->reg_widget_method(widtype, "long voodoo(string appname)", (void *)scope_voodoo);
	widman->build_script_lang(widtype, &gen_methods);
}


int init_scope(struct dope_services *d)
{
	widman  = (widman_services  *)(d->get_module("WidgetManager 1.0"));
	script  = (script_services  *)(d->get_module("Script 1.0"));
	hashtab = (hashtab_services *)(d->get_module("HashTable 1.0"));
	appman  = (appman_services  *)(d->get_module("ApplicationManager 1.0"));

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	gen_methods.get_type  = scope_get_type;
	gen_methods.free_data = scope_free_data;

	build_script_lang();

	d->register_module("Scope 1.0",&services);
	return 1;
}