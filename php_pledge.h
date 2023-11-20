/* php_pledge.h */

#define PHP_PLEDGE_EXTNAME "pledge"
#define PHP_PLEDGE_VERSION "2.1.2"

extern zend_module_entry pledge_module_entry;
#define phpext_pledge_ptr &check_pledge_entry

PHP_FUNCTION(pledge);
PHP_FUNCTION(unveil);

