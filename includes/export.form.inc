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
function islandora_db_export_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'sfudora', 'includes/admin.form');

  $form = array();
  $form['#tree'] = TRUE;
  
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
    '#ajax' => array(
      'callback' => 'islandora_db_export_update_mysql_tables',
      'wrapper' => 'tables-list-wrapper',
    ),

  );
  
  // Query to get all of the databases.
  $current_selected_db  = isset($form_state['values']['islandora_db_export_database']) ? $form_state['values']['islandora_db_export_database'] : $form['islandora_db_export_database']['#default_value'];
  
  $query = "SHOW TABLES FROM $current_selected_db";
  $results = mysql_query($query);
  $tables_list = array();
  while ($row = mysql_fetch_row($results)) {
    $tables_list[$row[0]] = array('table_name' =>$row[0]);
  }
  
  // Set the base export default values;
  $default_values = array();
  $base_tables = variable_get('islandora_db_export_base_export', $default_values);

  foreach ($tables_list as $table) {
    $default_values[$table['table_name']] = FALSE;
    if(in_array($table['table_name'], $base_tables)) {
      $default_values[$table['table_name']] = TRUE;
    }
  }
  
  $header = array(
    'table_name' => t('Table Name'),
  );
  $form['islandora_db_export_mysql_tables'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $tables_list,
    '#default_value'=> $default_values,
    '#empty' => t('No Tables for this database.'),
    '#prefix' => '<div id="tables-list-wrapper">',
    '#suffix' => '</div>',
  );

  // Set a specific callback for the submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Export Tables'),
  );
  
  
  return $form;
}
/**
 * Form submission function.
 */
function islandora_db_export_form_submit(&$form, &$form_state) {

  $server = variable_get('islandora_db_export_host','');
  $user = variable_get('islandora_db_export_user','');
  $pass = variable_get('islandora_db_export_password','');
  $connection = mysql_connect($server, $user, $pass);
  $current_selected_db  = isset($form_state['values']['islandora_db_export_database']) ? $form_state['values']['islandora_db_export_database'] : $form['islandora_db_export_database']['#default_value'];
  
  if (!file_exists('/tmp/dump.sql')) {
    $dump_file_path = drupal_realpath("temporary://dump.sql");
    $fp_dump = fopen($dump_file_path, 'w+');
    file_put_contents($dump_file_path, '');
    fclose($fp_dump);
  }
  $dump_file_path = '/tmp/dump.sql';
  $tables_string = '';
  foreach ($form_state['values']['islandora_db_export_mysql_tables'] as $table => $value){
    if($value) {
      $tables_string .= $value . ' ';
    }
  }

  $command = "mysqldump --user {$user} --password={$pass} {$current_selected_db} {$tables_string}> {$dump_file_path}";
  $output = array(); $ret = 0;
  exec($command, $output, $ret);
  if ($ret != 0) {
    dsm(t('Export failed'));
  }
  else {
    dsm(t('Export Succeeded '));
  }
  
  header('Content-type: application/octet-stream');
  header("Content-Disposition: attachment; filename=dump.sql");
  print_r(file_get_contents('/tmp/dump.sql'));
  exit();

    
}


function islandora_db_export_update_mysql_tables($form, &$form_state) {
  return $form['islandora_db_export_mysql_tables'];
}