<?php

/**
 * @file
 * Utility routines to load the Smart IP database.
 */

/******************************************************************************
 * Maxmind CSV format                                                         *
 ******************************************************************************/ 
/**
 * Prepare a batch definition
 *
 * This will download/extract CSV from maxmind.com and
 * update the Smart IP database.
 */
function smart_ip_update_db_batch($src = NULL) {
  if (empty($src)) {
    $src = smart_ip_get_csv_source_filename();
  }
  $operations = array();
  $operations[] = array('smart_ip_get_zip', array($src));
  $operations[] = array('smart_ip_extract_zip', array());
  $operations[] = array('smart_ip_store_location_csv', array());
  $operations[] = array('smart_ip_update_database', array());  
  $recover = variable_get('smart_ip_get_zip_done', FALSE) | variable_get('smart_ip_extract_zip_done', FALSE) | variable_get('smart_ip_store_location_csv_done', FALSE);
  $batch = array(
    'operations'       => $operations,
    'finished'         => 'smart_ip_update_db_batch_finished',
    // We can define custom messages instead of the default ones.
    'title'            => t('Processing download/extract CSV from maxmind.com and update Smart IP database'),
    'init_message'     => $recover ? t('Recovering...') : t('Starting...'),
    //'progress_message' => t('Elapsed: @elapsed.'),
    'error_message'    => t('Downloading/extracting CSV has encountered an error.'),
    'file'             => drupal_get_path('module', 'smart_ip') . '/includes/smart_ip.utility.inc',
  );
  return $batch;
}

function smart_ip_get_zip($src = NULL, &$context) {
  if (variable_get('smart_ip_store_location_csv_done', FALSE) || variable_get('smart_ip_extract_zip_done', FALSE)) {
    // We are in recovery mode.
    return;
  }
  $path = file_directory_path() . '/smart_ip';
  if (!file_check_directory($path, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    // Private file system path not defined then stop the process
    $message = t('File directory path is not writable, please check !here.',  array('!here' => l('here', 'admin/settings/file-system')));
    $context['results']['#abort'] = $message;
    $context['message'] = $message;
    return;
  }
  $zip_files = file_scan_directory($path, '\.zip$');
  foreach ($zip_files as $zip_file) {
    $context['results']['#zip_file'] = $zip_file->filename;
  }
  if (variable_get('smart_ip_get_zip_done', FALSE)) {
    // We are in recovery mode. Using previous downloaded zip.
    $context['finished'] = TRUE;
    $context['message']  = t('Using previous downloaded zip file (recovery mode)');
    return;
  }
  variable_set('smart_ip_get_zip_done', FALSE);
  if (empty($src)) {
    // Fallback zip file
    $src = 'http://geolite.maxmind.com/download/geoip/database/GeoLiteCity_CSV/GeoLiteCity_' . format_date(time(), 'custom', 'Ym') . '01.zip';
  }
  if (isset($context['results']['#zip_file'])) {
    // Don't download, use manually uploaded zip file then 
    // update our progress information.
    $context['finished'] = TRUE;
    $context['message']  = t('Using manually uploaded @zip file', array('@zip' => $context['results']['#zip_file']));
  }
  else {
    $context['results']['#zip_file'] = $path . '/geoip_db.zip';
    $context['finished'] = 50 / 100; 
    if (@copy($src, $context['results']['#zip_file'])) {
      // Update our progress information.
      $context['finished'] = TRUE; 
      $context['message']  = t('Download done. Extracting zip file...');
      // An indicator that is to be used in recovery mode
      variable_set('smart_ip_get_zip_done', TRUE);
    }
    else {
      // Error occured then stop the process
      $message = t('Download %src file failed. Be sure %src exists.',  array('%src' => $src));
      $context['results']['#abort'] = $message;
      $context['message'] = $message;
    }
  }
}

function smart_ip_extract_zip(&$context) {
  $path = file_directory_path() . '/smart_ip';
  if (variable_get('smart_ip_extract_zip_done', FALSE) || variable_get('smart_ip_store_location_csv_done', FALSE)) {
    // We are in recovery mode. Using previous extracted csv files.
    $context['finished'] = TRUE;
    $context['message']  = t('Using previous extracted csv files (recovery mode)');
    // Accumulate the previous extracted csv files.
    $csv_files = file_scan_directory($path, '\.csv$');
    foreach ($csv_files as $csv_file) {
      if (strpos($csv_file->filename, SMART_IP_LOCATION_CSV) !== FALSE) {
        $context['results']['#location_csv_file'] = $csv_file->filename;
      }
      elseif (strpos($csv_file->filename, SMART_IP_BLOCKS_CSV) !== FALSE) {
        $context['results']['#blocks_csv_file'] = $csv_file->filename;
      }
    }
    return;
  }
  variable_set('smart_ip_extract_zip_done', FALSE);
  // If this update was aborted in a previous step, or has a dependency that
  // was aborted in a previous step, go no further.
  if (isset($context['results']['#abort'])) {
    return;
  }
  $zip_file = $context['results']['#zip_file'];
  $zip      = new ZipArchive;
  $stat     = $zip->open($zip_file);
  $context['finished'] = 50 / 100; 
  if ($stat === TRUE) {
    // Delete existing csv files
    $csv_files = file_scan_directory($path, '\.csv$');
    foreach ($csv_files as $csv_file) {
      file_delete($csv_file->filename);
    }
    $zip->extractTo($path);
    $zip->close();
    file_delete($zip_file);
    unset($context['results']['#zip_file']);
    $csv_files = file_scan_directory($path, '\.csv$');
    foreach ($csv_files as $csv_file) {
      $csv_dir = str_replace($csv_file->basename, '', $csv_file->filename);
      file_move($csv_file->filename, $path . '/' . $csv_file->basename, FILE_EXISTS_REPLACE);
      if (strpos($csv_file->filename, SMART_IP_LOCATION_CSV) !== FALSE) {
        $context['results']['#location_csv_file'] = $path . '/' . $csv_file->basename;
      }
      elseif (strpos($csv_file->filename, SMART_IP_BLOCKS_CSV) !== FALSE) {
        $context['results']['#blocks_csv_file'] = $path . '/' . $csv_file->basename;
      }
      
    }
    file_delete($csv_dir . '*');
    @rmdir($csv_dir);
    // Update our progress information.
    $context['finished'] = TRUE;
    $context['message']  = t('geoip_db.zip extraction done. Starting the process of parsing extracted CSV files...');
    // The succeeding process can now be interrupted
    variable_set('smart_ip_db_update_busy', FALSE);
    // An indicator that is to be used in recovery mode
    variable_set('smart_ip_extract_zip_done', TRUE); 
  }
  else {
    // Error occured then stop the process
    $message = t('Unzip failed (error code: %code).',  array('%code' => $stat));
    $context['results']['#abort'] = $message;
    $context['message'] = $message;
    // Delete the corrupted zip file
    file_delete($zip_file);
    unset($context['results']['#zip_file']);
    // Set download process flag as undone to re-download the database from maxmind
    variable_set('smart_ip_get_zip_done', FALSE);
  }
}

function smart_ip_store_location_csv(&$context) {
  $last_cache   = variable_get('smart_ip_store_location_csv_done', FALSE);
  $cache_intact = cache_get('smart_ip:' . $last_cache, 'cache_smart_ip');
  if ($cache_intact) {
    // We are in recovery mode. Using previous stored locations.
    $context['finished'] = TRUE;
    $context['message']  = t('Using previous stored locations (recovery mode)'); 
    return;
  }
  variable_set('smart_ip_store_location_csv_done', FALSE);
  // If this update was aborted in a previous step, or has a dependency that
  // was aborted in a previous step, go no further.
  if (isset($context['results']['#abort'])) {
    return;
  }
  $fp = @fopen($context['results']['#location_csv_file'], 'r');
  if ($fp === FALSE) {
    // Error occured then stop the process
    $message = t('Opening CSV file %file failed.',  array('%file' => $context['results']['#location_csv_file']));
    $context['results']['#abort'] = $message;
    $context['message'] = $message;
    unset($context['results']['#location_csv_file']);
    variable_set('smart_ip_extract_zip_done', FALSE);
    variable_set('smart_ip_get_zip_done', FALSE);
  }
  else {
    if (!isset($context['sandbox']['#location_csv_pointer'])) {
      $location_csv_pointer = variable_get('smart_ip_location_csv_pointer', 0);
      if ($location_csv_pointer) {
        // Recover from the last interrupted pointer
        @fseek($fp, $location_csv_pointer);
      }
      else {
        // New update, clear the cache
        cache_clear_all('smart_ip:', 'cache_smart_ip', TRUE);
        // Record the last pointer
        $fp_check = @fopen($context['results']['#location_csv_file'], 'r');
        @fseek($fp_check, -1, SEEK_END);
        variable_set('smart_ip_location_csv_last_pointer', @ftell($fp_check));
      }
    }
    else {
      @fseek($fp, $context['sandbox']['#location_csv_pointer']);
    }
    $data = @fgetcsv($fp);
    $context['sandbox']['#location_csv_pointer'] = @ftell($fp);
    if (feof($fp)) {
      fclose($fp);
      // Update our progress information.
      $context['finished'] = TRUE;
      $context['message']  = t('Processing %location done', array('%location' => basename($context['results']['#location_csv_file'])));   
      unset($context['results']['#location_csv_file']);
      // An indicator that is to be used in recovery mode
      variable_set('smart_ip_store_location_csv_done', (int) variable_get('smart_ip_indicator_last_ip', NULL));
      // Reset our last IP indicator
      variable_set('smart_ip_indicator_last_ip', FALSE);
      variable_set('smart_ip_location_csv_pointer', 0);
      return;
    }
    else {
      $current_pointer    = $context['sandbox']['#location_csv_pointer'];
      $last_pointer       = variable_get('smart_ip_location_csv_last_pointer', 0);
      $estimated_progress = floor(100 * ($current_pointer / $last_pointer));
      // Update our progress information.
      $context['finished'] = $estimated_progress / 100;
      $context['message']  = t('Parsing %location at line number: @value of @end', array(
        '%location' => basename($context['results']['#location_csv_file']), 
        '@value'    => $current_pointer,
        '@end'      => $last_pointer,
      ));
    }
    if (count($data) == 9 && is_numeric($data[0])) {
      $data_location = array(
        'country_code' => $data[1], 
        'region'       => $data[2], 
        'city'         => $data[3], 
        'zip'          => $data[4], 
        'latitude'     => $data[5], 
        'longitude'    => $data[6],
      );
      variable_set('smart_ip_location_csv_pointer', $context['sandbox']['#location_csv_pointer']);
      variable_set('smart_ip_indicator_last_ip', $data[0]);
      cache_set('smart_ip:' . $data[0], $data_location, 'cache_smart_ip');
    }
  }
}

function smart_ip_update_database(&$context) {
  // If this update was aborted in a previous step, or has a dependency that
  // was aborted in a previous step, go no further.
  if (isset($context['results']['#abort'])) {
    return;
  }
  $fp = @fopen($context['results']['#blocks_csv_file'], 'r');
  if ($fp === FALSE) {
    unset($context['results']['#blocks_csv_file']);
    // Error occured then stop the process
    $message = t('Opening CSV file %file failed.',  array('%file' => $context['results']['#blocks_csv_file']));
    $context['results']['#abort'] = $message;
    $context['message'] = $message;
    unset($context['results']['#blocks_csv_file']);
    variable_set('smart_ip_extract_zip_done', FALSE);
    variable_set('smart_ip_get_zip_done', FALSE);
    variable_set('smart_ip_store_location_csv_done', FALSE);
  }
  else {
    if (!isset($context['sandbox']['#blocks_csv_pointer'])) {
      $blocks_csv_pointer = variable_get('smart_ip_blocks_csv_pointer', 0);
      if ($blocks_csv_pointer) {
        @fseek($fp, $blocks_csv_pointer);
      }
      else {
        if (db_table_exists('smart_ip_update_table')) {
          // The temporary working table for updating Smart IP database already exist, truncate it.
          db_query('TRUNCATE TABLE {smart_ip_update_table}');
        }
        else {
          module_load_install('smart_ip');
          // Add temporary working table for updating Smart IP database.
          $ret = array();
          db_create_table($ret, 'smart_ip_update_table', smart_ip_schema_definition_array());
        }
        // Record the last pointer
        $fp_check = @fopen($context['results']['#blocks_csv_file'], 'r');
        @fseek($fp_check, -1, SEEK_END);
        variable_set('smart_ip_blocks_csv_last_pointer', @ftell($fp_check));
      }
    }
    else {
      @fseek($fp, $context['sandbox']['#blocks_csv_pointer']);
    }
    $data = @fgetcsv($fp);
    $context['sandbox']['#blocks_csv_pointer'] = @ftell($fp);
    if (@feof($fp)) {
      @fclose($fp);
      // Update our progress information.
      $context['finished'] = TRUE;
      $context['message']  = t('Processing %blocks done', array('%blocks' => basename($context['results']['#blocks_csv_file'])));
      unset($context['results']['#blocks_csv_file']);
      // Tasks completed. Reset the recovery mode indicators.
      variable_set('smart_ip_get_zip_done', FALSE);
      variable_set('smart_ip_extract_zip_done', FALSE);
      variable_set('smart_ip_store_location_csv_done', FALSE);
    }
    else {
      $current_pointer    = $context['sandbox']['#blocks_csv_pointer'];
      $last_pointer       = variable_get('smart_ip_blocks_csv_last_pointer', 0);
      $estimated_progress = floor(100 * ($current_pointer / $last_pointer)) - 2;
      // Update our progress information.
      $context['finished'] = $estimated_progress / 100;
      $context['message']  = t('Parsing %blocks at line number: @value of @end', array(
        '%blocks' => basename($context['results']['#blocks_csv_file']),
        '@value'  => $current_pointer,
        '@end'    => $last_pointer,
      ));
    }
    if (count($data) == 3 && is_numeric($data[2])) {
      variable_set('smart_ip_blocks_csv_pointer', $context['sandbox']['#blocks_csv_pointer']);
      $location = cache_get('smart_ip:' . $data[2], 'cache_smart_ip');
      if (isset($location->data['country_code'])) {
        try {
          // Insert GeoIP into the temporary working Smart IP database table
          db_query("INSERT INTO {smart_ip_update_table} (geoip_id, ip_ref, country_code, region, city, zip, latitude, longitude) VALUES (%d, %d, '%s', '%s', '%s', '%s', %f, %f)", 
            $data[2],
            min($data[0], $data[1]),
            strtoupper($location->data['country_code']),
            strtoupper($location->data['region']),
            $location->data['city'],
            $location->data['zip'],
            $location->data['latitude'],
            $location->data['longitude']
          );
        }
        catch(Exception $error) {
          db_query("UPDATE {smart_ip_update_table} SET geoip_id = %d, country_code = '%s', region = '%s', city = '%s', zip = '%s', latitude = %f, longitude = %f WHERE ip_ref = %d", 
            $data[2], 
            strtoupper($location->data['country_code']), 
            strtoupper($location->data['region']), 
            $location->data['city'], 
            $location->data['zip'], 
            $location->data['latitude'], 
            $location->data['longitude'], 
            min($data[0], $data[1])
          );
        }
      }
    }
    // This lines of code should be here to ensure cache_get()
    // above will return non-empty value
    if ($context['finished'] === TRUE) {
      $ret = array();
      // Clear the Smart IP production table
      db_drop_table($ret, 'smart_ip');
      // Rename temporary working Smart IP database table to production table name {smart_ip}
      db_rename_table($ret, 'smart_ip_update_table', 'smart_ip'); // 'RENAME TABLE {smart_ip_update_table} TO {smart_ip}'
      cache_clear_all('smart_ip:', 'cache_smart_ip', TRUE);
      variable_set('smart_ip_blocks_csv_pointer', 0);
      variable_set('smart_ip_last_update', time());
      watchdog('smart_ip', 'Smart IP Database successfuly updated from maxmind.com.');
    }
  }
}

/**
 * Update Smart IP database batch 'finished' callback
 */
function smart_ip_update_db_batch_finished($success, $results, $operations) {
  if ($success) {
    if (isset($results['#abort'])) {
      // Set busy indicator to FALSE so that it can continue the 
      // process for the next try
      variable_set('smart_ip_db_update_busy', FALSE);
      drupal_set_message($results['#abort'], 'error');
    }
    else {
      drupal_set_message(t('Smart IP database sucessfully updated.'));
    }    
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    drupal_set_message(t('Error occurred while processing @operation with arguments : @args', array('@operation' => $error_operation[0], '@args' => print_r($error_operation[0], TRUE))), 'error');
  }
}

/******************************************************************************
 * Maxmind binary format                                                      *
 ******************************************************************************/
/**
 * Download a Maxmind binary database and activate it for use
 */
function smart_ip_maxmind_bin_db_update($license = '', $cron_run = FALSE, $force = FALSE) {
  // Only update the binary database when it is out of date or under specific direction
  if (!$force && smart_ip_maxmind_bin_db_up_to_date()) {
    return FALSE;
  }

  $path = file_directory_path() . '/smart_ip';
  $gz_name = $path . '/' . SMART_IP_BIN_DB_NAME . '.tar.gz';
  if (!file_check_directory($path, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    // Private file system path not defined then stop the process
    drupal_set_message(t('File directory path is not writable, please check !here.',  array('!here' => l('here', 'admin/settings/file-system'))), 'error');
    return FALSE;
  }

  $url = 'http://download.maxmind.com/app/geoip_download?' . http_build_query(array(
    'edition_id' => '133',
    'suffix' => 'tar.gz',
    'license_key' => $license ? $license : variable_get('smart_ip_maxmind_license', ''),
  ));
  
  if (file_put_contents($gz_name, fopen($url, 'r')) === FALSE) {
  	variable_set('smart_ip_maxmind_bin_db_update_error', 'Maxmind bin DB download failed');
  	watchdog('smart_ip', 'Maxmind bin DB download failed', array(), WATCHDOG_WARNING);
  	return FALSE;
  }

  try {
    $p = new PharData($gz_name);
    $p->extractTo($path . '/db_tmp');
    
    $files = file_scan_directory($path . '/db_tmp', SMART_IP_BIN_DB_NAME . '.dat');
    if (count($files) !== 1) {
      throw new Exception('Unable to determing the contents of the db_tmp directory (' . $path . '/db_tmp)');
    }

    foreach ($files as $file) {
      if (rename($file->filename, $path . '/' . SMART_IP_BIN_DB_NAME . '.dat') === FALSE) {
        throw new Exception('Failed to activate the downloaded database');
      }
    }
  }
  catch (Exception $e) {
    $msg = 'Error during database download/extraction: ' . $e->getMessage();
    variable_set('smart_ip_maxmind_bin_db_update_error', $msg);
    watchdog('smart_ip', $msg, array(), WATCHDOG_WARNING);
    return FALSE;
  }

  variable_set('smart_ip_maxmind_bin_db_update_error', NULL);
  variable_set('smart_ip_maxmind_bin_db_last_update', time());

  if ($cron_run) {
    watchdog('smart_ip', 'The Maxmind binary database has been updated via cron.');   
  }
  else {
    watchdog('smart_ip', 'The Maxmind binary database has been manually updated.');
  }

  file_delete($gz_name);
  rmdir_recurse("$path/db_tmp");

  return TRUE;
}

/**
 * Maxmind updates the DB every Tuesday, and we download every Wednesday.
 * That means that we only want to download if the current DB was downloaded
 * prior to the most recently available version.
 */
function smart_ip_maxmind_bin_db_up_to_date() {
  // backtrack our time calculations
  $time = strtotime('midnight');
  $dow = date('w', $time) - 3; // D-day (download day)
  if ($dow > 0) {
    $dow += 4;
  }
  $time_of_most_recent_db = $time - ($dow * 86400);
  $bin_last_update_time = strtotime('midnight', variable_get('smart_ip_maxmind_bin_db_last_update', 0));

  if ($bin_last_update_time < $time_of_most_recent_db) {
    return FALSE;
  }
  return TRUE;
}


function rmdir_recurse($path) {
  $path   = rtrim($path, '/').'/';
  $handle = opendir($path);
  while ($file = readdir($handle) !== FALSE) {
    if($file != '.' && $file != '..' ) {
      $fullpath = $path . $file;
      if(is_dir($fullpath)) {
        rmdir_recurse($fullpath);
      }
      else {
        unlink($fullpath);
      }
    }
  }
  closedir($handle);
  rmdir($path);
}