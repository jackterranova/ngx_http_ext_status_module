## ngx_http_module_ext_status_module ##

A custom nginx module to expose status metrics that are NOT exposed by the existing Stub Status module (ngx_http_stub_status_module).  

These metrics can then be referenced in an nginx config file for easy logging configuration.

_Exposes the following global variables_

* ngx_stat_accepte exposed as `connections_accepted`
* ngx_stat_handled exposed as `connections_handled`
* ngx_stat_requests exposed as `connections_requested`

An example nginx config

```
log_format my_format 'nginx.connections_active:$connections_accepted'
                     '\n'
                     'nginx.connections_reading:$connections_handled'
                     '\n'
                     'nginx.connections_waiting:$connections_requested'
```

## Compiling as a dynamic module ##

In general nginx modules are staically compiled along with the core nginx source code.

nginx does dupport `dynamic` modules where the modules can be included at startup in the nginx config via the `add_module` config directive.

At this time, this module can not be included as a dynamic module.  The issues encoutered is similar to that described in https://github.com/openresty/headers-more-nginx-module/issues/55.  It is not clear what makes a module 'support' dynamic inclusion.  Hopefully this can be remedied with help from the nginx mailing list.

For the time being, this module must be statically compiled along with nginx core source code.

## How to build  ##

Example script below.  Modify paths appropriately for you local install.  You may also want to verify the PGP keys of the nginx source tar ball.

In this example, your local nginx install should be found in `/usr/sbin/nginx`.
  test   
```
NGINX_VERSION=1.13.1

CONFIG="\
		--prefix=/etc/nginx \
		--sbin-path=/usr/sbin/nginx \
		--modules-path=/usr/lib/nginx/modules \
		--conf-path=/etc/nginx/nginx.conf \
		--error-log-path=/var/log/nginx/error.log \
		--http-log-path=/var/log/nginx/access.log \
		--pid-path=/var/run/nginx.pid \
		--lock-path=/var/run/nginx.lock \
		--http-client-body-temp-path=/var/cache/nginx/client_temp \
		--http-proxy-temp-path=/var/cache/nginx/proxy_temp \
		--http-fastcgi-temp-path=/var/cache/nginx/fastcgi_temp \
		--http-uwsgi-temp-path=/var/cache/nginx/uwsgi_temp \
		--http-scgi-temp-path=/var/cache/nginx/scgi_temp \
		--user=nginx \
		--group=nginx \
		--with-http_ssl_module \
		--with-http_realip_module \
		--with-http_addition_module \
		--with-http_sub_module \
		--with-http_dav_module \
		--with-http_flv_module \
		--with-http_mp4_module \
		--with-http_gunzip_module \
		--with-http_gzip_static_module \
		--with-http_random_index_module \
		--with-http_secure_link_module \
		--with-http_stub_status_module \
		--with-http_auth_request_module \
		--with-http_xslt_module=dynamic \
		--with-http_image_filter_module=dynamic \
		--with-http_geoip_module=dynamic \
		--with-threads \
		--with-stream \
		--with-stream_ssl_module \
		--with-stream_ssl_preread_module \
		--with-stream_realip_module \
		--with-stream_geoip_module=dynamic \
		--with-http_slice_module \
		--with-mail \
		--with-mail_ssl_module \
		--with-compat \
		--with-file-aio \
		--with-http_v2_module \
		--add-module=/ngx_http_ext_status_module \
	" \

  # wget works here too
  curl https://nginx.org/download/nginx-$NGINX_VERSION.tar.gz -o nginx.tar.gz
  tar -zxC /usr/src -f nginx.tar.gz
  rm nginx.tar.gz
  cd /usr/src/nginx-$NGINX_VERSION
  ./configure $CONFIG --with-debug
  make
  make install
```
