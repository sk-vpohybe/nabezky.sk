<?php

class location_handler_sort_location_country extends views_handler_sort {

    function option_definition() {
        $options = parent::option_definition();
        $options['country_sort'] = array('default' => 'name');
        return $options;
    }

    function has_extra_options() {
        return TRUE;
    }

    function extra_options_form(&$form, &$form_state) {
        //drupal_set_message('location_handler_sort_location_country inside extra_options_form');
        $form['country_sort'] = array(
            '#type' => 'radios',
            '#title' => t('Name or Code'),
            '#options' => array(
                'name' => t("Use the County Name (as specified by the database)."),
                'code' => t("Use the ISO3166 two letter country code ."),
            ),
            '#description' => t("This sets how country will be sorted, either by Name or the ISO 3166 2 letter code"),
            '#default_value' => $this->options['country_sort'],
        );
    }

    function query() {
        //drupal_set_message('location_handler_sort_location_country inside query');

        //Make sure the table_alias work.  This sets up some of the list pointers.
        $this->ensure_my_table();

        //This is needed otherwise the the add_relationship will not work.
        $this->query->ensure_path($this->table_alias);

        //drupal_set_message('Options:' . print_r($this->options,1));
        if ($this->options['country_sort']=='name') {
            //drupal_set_message('Select sort by name');
            $join = new views_join();
            $join->definition = array(
                'table' => 'location_country',
                'field' => 'code',
                'left_table' => $this->table_alias,
                'left_field' => 'country',
            );
            if (!empty($this->options['required'])) {
                $join->definition['type'] = 'INNER';
            }
            $join->construct();

            $this->query->add_relationship('country_name',$join, $this->definition['base'], NULL);
            $this->query->add_orderby('country_name','name', $this->options['order'],'country_sort');
        }  elseif ($this->options['country_sort']=='code') {
            //drupal_set_message('Select sort by code');
            //node_users__location.country
            $this->query->add_orderby($this->table_alias,'country', $this->options['order'],'country_code_sort');
        }


        //Create the join definition with the location_county table that has the mapping from
        //country code to country name
    }
}