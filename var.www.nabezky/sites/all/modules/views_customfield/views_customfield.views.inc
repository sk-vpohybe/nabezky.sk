<?php

/**
 * Implementation of hook_views_handlers().
 *
 * @return array
 */
function views_customfield_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'views_customfield') .'/includes',
    ),
    'handlers' => array(
      'views_customfield_handler_field_markup' => array(
        'parent' => 'views_handler_field',
      ),
      'views_customfield_handler_field_phpcode' => array(
        'parent' => 'views_handler_field',
      ),
      'views_customfield_handler_field_rownumber' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_data().
 *
 * @return array
 */
function views_customfield_views_data() {
  $data['customfield']['table']['group'] = t('Customfield');
  $data['customfield']['table']['join'] = array(
    '#global' => array(),
  );
  
  $data['customfield']['markup'] = array(
    'title' => t('Markup'),
    'help' => t('Display custom markup text.'),
    'field' => array(
      'handler' => 'views_customfield_handler_field_markup',
      'click sortable' => FALSE,
      'notafield' => TRUE,
    ),
  );

  $data['customfield']['phpcode'] = array(
    'title' => t('PHP code'),
    'help' => t('Display row-specific custom text using PHP.'),
    'field' => array(
      'handler' => 'views_customfield_handler_field_phpcode',
      'click sortable' => FALSE,
      'notafield' => TRUE,
    ),
  );
  
  $data['customfield']['rownumber'] = array(
    'title' => t('Rownumber'),
    'help' => t('Display rownumber.'),
    'field' => array(
      'handler' => 'views_customfield_handler_field_rownumber',
      'click sortable' => FALSE,
      'notafield' => TRUE,
    ),
  );
  
  return $data;
}

/**
 * Implementation of hook_views_plugins().
 */
function views_customfield_views_plugins() {
  if (!module_exists('views_calc')) return array();

  return array(
    'style' => array(
      'views_customfield_calc_parent' => array(
        // this isn't really a display but is necessary so the file can
        // be included.
        'no ui' => TRUE,
        'handler' => 'views_calc_table',
        'path' => drupal_get_path('module', 'views_calc'),
        'parent' => '',
      ),
      'views_customfield_calc' => array(
        'title' => t('Views Custom Field Calc Table'),
        'help' => t('Creates a table with column calculations (including custom fields).'),
        'handler' => 'views_customfield_calc_table',
        'path' => drupal_get_path('module', 'views_customfield') .'/includes',
        'parent' => 'views_calc',
        'theme' => 'views_calc_table',
        'theme file' => 'theme.inc',
        'theme path' => drupal_get_path('module', 'views_calc'),
        'uses row plugin' => FALSE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'even empty' => FALSE,
      ),
    ),
  );
}

