/* php_pledge.h */

#define PHP_PLEDGE_EXTNAME "pledge"
#define PHP_PLEDGE_VERSION "1.0.0"

extern zend_module_entry pledge_module_entry;
#define phpext_pledge_ptr &check_pledge_entry

PHP_FUNCTION(pledge);

