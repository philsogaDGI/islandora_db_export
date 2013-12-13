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
function islandora_db_export_import_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'sfudora', 'includes/admin.form');

  $form = array();
  $form['#tree'] = TRUE;
  
  $form['islandora_db_export_import']['islandora_db_export_import_file'] = array(
    '#type' => 'managed_file',
    '#title' => t('Upload a .sql file.'),
    '#description' => t('Upload a .sql file'),
    '#upload_validators' => array('file_validate_extensions' => array('sql')),
  );
  
  // Create a connection to mysql based on admin settings.
  $server = variable_get('islandora_db_export_host','');
  $user = variable_get('islandora_db_export_user','');
  $pass = variable_get('islandora_db_export_password','');
  $connection = mysql_connect($server, $user, $pass);
  
  // Query to get all of the databases.
  $query = "SHOW DATABASES";
  $results = mysql_query($query);
  $databases_list = array();
  while ($row = mysql_fetch_row($results)) {
    $databases_list[$row[0]] = $row[0];
  }
  // Get the active database.
  $active_db = db_query('select database() as base')->fetchObject()->base;

  $form['islandora_db_export_database'] = array(
    '#type' => 'select',
    '#title' => t('Select database'),
    '#default_value' => "$active_db",
    '#options' => $databases_list,
  );
  
  // Set a specific callback for the submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import Tables'),
  );
  
  
  return $form;
}
/**
 * Form submission function.
 */
function islandora_db_export_import_form_submit(&$form, &$form_state) {
dsm($form);
dsm($form_state);

  $server = variable_get('islandora_db_export_host','');
  $user = variable_get('islandora_db_export_user','');
  $pass = variable_get('islandora_db_export_password','');
  $connection = mysql_connect($server, $user, $pass);
  $current_selected_db  = isset($form_state['values']['islandora_db_export_database']) ? $form_state['values']['islandora_db_export_database'] : $form['islandora_db_export_database']['#default_value'];

  $file = file_load($form_state['values']['islandora_db_export_import']['islandora_db_export_import_file']);
  dsm($file);
  
  $command = "mysql --user {$user} --password={$pass} {$current_selected_db} < /tmp/{$file->filename}";
 dsm($command);
  $output = array(); $ret = 0;
  exec($command, $output, $ret);
  if ($ret != 0) {
    dsm(t('Import failed'));
  }
  else {
    dsm(t('Import Succeeded '));
  }
  
    
}


