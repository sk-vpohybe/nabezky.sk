<?php

/**
 * @file
 * Views integration for Advanced Poll module.
 */

/**
 * Implementation of hook_views_data()
 */
function advpoll_views_data() {
    // Basic table information.
    $data['advpoll']['table']['group'] = t('Advanced Poll');

    // Join to 'node' as a base table.
    $data['advpoll']['table']['join'] = array(
        'node' => array(
            'left_field' => 'nid',
            'field' => 'nid',
        ),
    );

    // ----------------------------------------------------------------
    // Fields

    $data['advpoll']['active'] = array(
        'title' => t('Active'),
        'help' => t('Whether the poll is open for voting.'),
        'field' => array(
            'handler' => 'views_handler_field_boolean',
            'click sortable' => TRUE,
        ),
        'filter' => array(
            'handler' => 'views_handler_filter_boolean_operator',
            'label' => t('Active'),
            'type' => 'yes-no',
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
    );
    //MW added to allow display/sorting in views
    $data['advpoll']['create_view_block'] = array(
        'title' => t('Create Block'),
        'help' => t('Whether or not to automatically build a block for a given poll.'),
        'field' => array(
            'handler' => 'views_handler_field_boolean',
            'click sortable' => TRUE,
        ),
        'filter' => array(
            'handler' => 'views_handler_filter_boolean_operator',
            'label' => t('Create View Block'),
            'type' => 'yes-no',
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
    );    

     //MW added to allow display/sorting in views
    $data['advpoll']['footer_message'] = array(
        'title' => t('Poll Footer Message'),
        'help' => t('Displays optional poll footer message.'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
        'filter' => array(
            'handler' => 'views_handler_filter_string',
        ),
        'argument' => array(
            'handler' => 'views_handler_argument_string',
        ),
    );
   
    
    
    $data['advpoll']['start_date'] = array(
        'title' => t('Start date'),
        'help' => t('The date at which voting begins for the poll.'),
        'field' => array(
            'handler' => 'views_handler_field_date',
            'click sortable' => TRUE,
        ),
        'sort' => array(
            'handler' => 'views_handler_sort_date',
        ),
        'filter' => array(
            'handler' => 'views_handler_filter_date',
        ),
    );

    $data['advpoll']['end_date'] = array(
        'title' => t('End date'),
        'help' => t('The date at which voting ends for the poll.'),
        'field' => array(
            'handler' => 'views_handler_field_date',
            'click sortable' => TRUE,
        ),
        'sort' => array(
            'handler' => 'views_handler_sort_date',
        ),
        'filter' => array(
            'handler' => 'views_handler_filter_date',
        ),
    );

    $data['advpoll']['question'] = array(
        'title' => t('Question'),
        'help' => t('Optional text to display as the subject of the poll.'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
        'filter' => array(
            'handler' => 'views_handler_filter_string',
        ),
        'argument' => array(
            'handler' => 'views_handler_argument_string',
        ),
    );

    return $data;
}

