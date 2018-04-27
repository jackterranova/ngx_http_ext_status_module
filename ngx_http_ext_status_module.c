#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static ngx_int_t ngx_http_ext_status_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static char *ngx_http_set_ext_status(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_ext_status_add_variables(ngx_conf_t *cf);

static ngx_command_t  ngx_http_ext_status_commands[] = {

    { ngx_string("ext_status"),
      NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS|NGX_CONF_TAKE1,
      ngx_http_set_ext_status,
      0,
      0,
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_ext_status_module_ctx = {
    ngx_http_ext_status_add_variables,     /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};


ngx_module_t  ngx_http_ext_status_module = {
    NGX_MODULE_V1,
    &ngx_http_ext_status_module_ctx,       /* module context */
    ngx_http_ext_status_commands,          /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

/*

  Variables added to the nginx context via ngx_http_ext_status_add_variables
  during preconfig.

  These variable expose global variables in the nginx config.  See getter method
  ngx_http_ext_status_variable for how the exposed config name is mapped to the
  global variable it represents.

  Each variable is a struct of type ngx_http_variable_t with the following def ...

    name
    setter
    getter
    data
    flags
    index

    The "data" is simply the index position of the variable in the
    modules variable array "ngx_http_ext_status_vars".

 */
static ngx_http_variable_t  ngx_http_ext_status_vars[] = {
    {
      ngx_string("connections_accepted"), /* name                 */
      NULL,                               /* setter               */
      ngx_http_ext_status_variable,       /* getter               */
      0,                                  /* data (i.e. position) */
      NGX_HTTP_VAR_NOCACHEABLE,           /* flags                */
      0                                   /* index                */
    },

    {
      ngx_string("connections_handled"),
      NULL,
      ngx_http_ext_status_variable,
      1,
      NGX_HTTP_VAR_NOCACHEABLE,
      0
    },

    {
      ngx_string("connections_requested"),
      NULL,
      ngx_http_ext_status_variable,
      2,
      NGX_HTTP_VAR_NOCACHEABLE,
      0
    },

    ngx_http_null_variable
};

static ngx_int_t
ngx_http_ext_status_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char            *p;
    ngx_atomic_int_t   value;

    p = ngx_pnalloc(r->pool, NGX_ATOMIC_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    switch (data) {
    case 0:
        value = *ngx_stat_accepted;
        break;

    case 1:
        value = *ngx_stat_handled;
        break;

    case 2:
        value = *ngx_stat_requests;
        break;

    /* suppress warning */
    default:
        value = 0;
        break;
    }

    v->len = ngx_sprintf(p, "%uA", value) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}

static ngx_int_t
ngx_http_ext_status_add_variables(ngx_conf_t *cf)
{
  ngx_http_variable_t  *var, *v;

  for (v = ngx_http_ext_status_vars; v->name.len; v++) {
    var = ngx_http_add_variable(cf, &v->name, v->flags);
    if (var == NULL) {
      return NGX_ERROR;
    }

    var->get_handler = v->get_handler;
    var->data = v->data;
  }

  return NGX_OK;
}

static char *
ngx_http_set_ext_status(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    // Currently don't need a handler
    clcf->handler = NULL;

    return NGX_CONF_OK;
}
