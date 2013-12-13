<?php

/**
 * @file
 * Form handling for the administration of islandora db export.
 */

/**
 * Defines the Simon Fraser University admin form.
 *
 * @param array $form
 *   An array representing a Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   An array representing the Simon Fraser University admin form.
 */
function islandora_db_export_admin_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'sfudora', 'includes/admin.form');

  $form = array();
  $form['#tree'] = TRUE;

  $form['mysql_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('MySQL Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
   $form['base_export_wrapper'] = array(
    '#type' => 'markup',
  );

  $form['mysql_settings']['islandora_db_export_host'] = array(
    '#type' => 'textfield',
    '#title' => t('MySQL Host'),
    '#default_value' => variable_get('islandora_db_export_host', 'localhost:3306'),
    '#description' => t('MySQL server and port: (localhost:3306)'),
    '#required' => TRUE,
  );

  $form['mysql_settings']['islandora_db_export_user'] = array(
    '#type' => 'textfield',
    '#title' => t('MySQL User'),
    '#default_value' => variable_get('islandora_db_export_user', ''),
    '#description' => t('MySQL User'),
    '#required' => TRUE,
  );

  $form['mysql_settings']['islandora_db_export_password'] = array(
    '#type' => 'password',
    '#title' => t('MySQL Password'),
    '#default_value' => variable_get('islandora_db_export_password', ''),
    '#description' => t('MySQL User'),
    '#required' => TRUE,
  );
  
  $default_values = array(
    'block',
    'block_custom',
    'block_node_type',
    'block_role',
  
  );
  $form['base_export_wrapper']['islandora_db_export_base_export'] = array(
      '#type' => 'textarea',
      '#title' => t('Base Export tables'),
      '#default_value' => implode("\n", variable_get('islandora_db_export_base_export', $default_values)),
      '#required' => FALSE,
    );
  
  // Set a specific callback for the submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Configuration'),
  );

  return $form;
}

/**
 * Form validation function.
 */
function islandora_db_export_admin_form_validate(&$form, &$form_state) {

  $server = $form_state['values']['mysql_settings']['islandora_db_export_host'];
  $user = $form_state['values']['mysql_settings']['islandora_db_export_user'];
  $pass = $form_state['values']['mysql_settings']['islandora_db_export_password'];
  $connection = mysql_connect($server, $user, $pass);
  if ($connection) {
    drupal_set_message(t('Connected to MySQL.'), 'status');
  }

}

/**
 * Form submission function.
 */
function islandora_db_export_admin_form_submit(&$form, &$form_state) {

  foreach ($form_state['values']['base_export_wrapper'] as $variable => $value) {
   $value = str_replace(" ","",$value);
   $values=array_map('trim',explode("\n", $value));
    variable_set($variable, $values);
  }
  foreach ($form_state['values']['mysql_settings'] as $variable => $value) {
    variable_set($variable, $value);
  }
  drupal_set_message(t('MySQL settings saved.'), 'status');
}
