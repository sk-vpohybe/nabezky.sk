<?php

function _trackfield_map_admin() {
	$trackfield_format_sizes = variable_get('trackfield_map_format_sizes', array());
	/* Add a blank line for new entries */
	$trackfield_format_sizes[''] = array();
	
	$form['sizes'] = array(
		'#type' => 'fieldset',
		'#title' => t('Map sizes'),
		'#tree' => TRUE,
		'#theme' => 'trackfield_map_sizes_form',
		'#description' => t('Various size mapping options are available whenever these are shown (in teaser, full node, views, etc). Define the possible choices required here.')
	);
	
	$lp = 0;
	$type_options = array('static_image' => 'Built in Static', 'leaflet' => 'Leatlet', 'gmap_static' => 'Google Maps Static');
	if (module_exists('gmap')) {
		$type_options['gmap'] = 'Google Maps';
	}
	foreach ($trackfield_format_sizes as $key => $config) {
		$form['sizes'][$lp]['del'] = array(
			'#type' => 'checkbox',
		);
		$form['sizes'][$lp]['label'] = array(
			'#type' => 'textfield',
			'#default_value' => $key,
			'#size' => 15,
			'#maxlength' => 32,
		);
		$form['sizes'][$lp]['width'] = array(
			'#type' => 'textfield',
			'#default_value' => $config['width'],
			'#size' => 4,
			'#maxlength' => 5,
		);
		$form['sizes'][$lp]['height'] = array(
			'#type' => 'textfield',
			'#default_value' => $config['height'],
			'#size' => 4,
			'#maxlength' => 5,
		);
		$form['sizes'][$lp]['type'] = array(
			'#type' => 'select',
			'#default_value' => $config['type'],
			'#options' => $type_options,
		);
		$form['sizes'][$lp]['gmap_attr'] = array(
			'#type' => 'textfield',
			'#default_value' => $config['gmap_attr'],
			'#size' => 32,
			'#maxlength' => 128,
		);
		$lp++;
	}
	
	$form['buttons']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration') );
	$form['buttons']['delete'] = array('#type' => 'submit', '#value' => t('Delete Selected'));
	$form['#submit'] = array('trackfield_map_sizes_submit');
	$form['#validate'] = array('trackfield_map_sizes_validate');
	
	return $form;
}

function theme_trackfield_map_sizes_form($form) {
	$header = array(t('Del'), t('Label'), t('Width'), t('Height'), t('Type'), t('Attributes'));
	foreach (element_children($form) as $key) {
		$row = array();
		$row[] = drupal_render($form[$key]['del']);
		$row[] = drupal_render($form[$key]['label']);
		$row[] = drupal_render($form[$key]['width']);
		$row[] = drupal_render($form[$key]['height']);
		$row[] = drupal_render($form[$key]['type']);
		$row[] = drupal_render($form[$key]['gmap_attr']);
		$rows[] = $row;
	}
	$output .= theme('table', $header, $rows);
	$output .= drupal_render($form);
	
	return $output;
}

function trackfield_map_sizes_validate($form, &$form_state) {
	/* Don't bother if just deleting what is marked */
	if ($form_state['values']['op'] == t('Delete Selected')) return;
	
	foreach ($form_state['values']['sizes'] as $key => $config) {
		/* Only check if not empty */
		if (!empty($config['label'])) {
			/* width/height/weight must be > 0 */
			if ($config['width'] < 1) {
				form_set_error("sizes][$key][width", t('Width must be a positive integer.'));
			}
			if ($config['height'] < 1) {
				form_set_error("sizes][$key][height", t('Height must be a positive integer.'));
			}
		}
	}
}

function trackfield_map_sizes_submit($form, &$form_state) {
	if ($form_state['values']['op'] == t('Delete Selected')) {
		/* Load the settings and remove the ones checked for delete */
		$deleted = 0;
		$trackfield_format_sizes = variable_get('trackfield_map_format_sizes', array());
		foreach ($form_state['values']['sizes'] as $config) {
			if ($config['del']) {
				$key = $config['label'];
				if (array_key_exists($key, $trackfield_format_sizes)) {
					unset($trackfield_format_sizes[$key]);
					$deleted++;
				}
			}
		}
		if ($deleted) {
			variable_set('trackfield_map_format_sizes', $trackfield_format_sizes);
			drupal_set_message(t('@deleted configuration value@s deleted.', array('@deleted' => $deleted, '@s' => $deleted > 1 ? 's' : '')));
			/* Clear CCK cache to pick up changed sizes */
			content_clear_type_cache();
		} else {
			drupal_set_message(t('No valid options to delete.'));
		}
		
	} else {
		/* Simply build array based on form_values and save it */
		$trackfield_format_sizes = array();
		foreach ($form_state['values']['sizes'] as $lp => $config) {
			if (!empty($config['label'])) {
				$key = $config['label'];
				/* Remove items we don't need to save */
				unset($config['del']);
				unset($config['label']);
				/* And change margins to array */
				$trackfield_format_sizes[$key] = $config;
			}
		}
		variable_set('trackfield_map_format_sizes', $trackfield_format_sizes);
		
		/* Clear CCK cache to pick up changed sizes */
		content_clear_type_cache();
		
		drupal_set_message(t('The configuration options have been saved.'));
	}
}