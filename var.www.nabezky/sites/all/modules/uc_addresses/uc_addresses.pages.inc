<?php
/**
 * @file
 * Page callbacks for viewing, adding, editing, and deleting addresses.
 */

// -----------------------------------------------------------------------------
// VIEW ADDRESSES
// -----------------------------------------------------------------------------

/**
 * Generate a list of one or all addresses defined by one user and
 * then theme the list for display.
 *
 * If the current user can edit the addresses, then provide an edit
 * link for each address.
 *
 * @param object $address_user
 *   The user whose address book we want to display.
 * @return string
 *   The themed address book.
 */
function uc_addresses_address_book($address_user) {
  drupal_add_css(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.css');
  drupal_set_title(t('Address book'));

  // Initialize addresses array used for in the template.
  $addresses = array();
  try {
    // Get all addresses for address user
    $address_book = UcAddressesAddressBook::get($address_user);
    $all_addresses = $address_book->getAddresses();

    if (count($all_addresses) > 0) {
      // Check for each address if the user may view it.
      foreach ($all_addresses as $address) {
        if ($address_output = uc_addresses_list_one_address($address_user, $address)) {
          $addresses[$address->getId()] = $address_output;
        }
      }
    }
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
  }

  $options = array();
  // Check if user may add addresses
  if (UcAddressesPermissions::canEditAddress($address_user)) {
    // Show add link
    $options['add_link'] = TRUE;
  }

  return theme('uc_addresses_address_book', $addresses, $address_book, $options);
}

/**
 * Prepare variables for address book
 *
 * @param array $vars
 *   An array with the following:
 *   - addresses (array of UcAddressesAddress instances)
 *   - address_book' (instance of UcAddressesAddressBook)
 *   - options (array):
 *     - add_link (boolean): whether or not display an add address link.
 *
 * @return void
 */
function template_preprocess_uc_addresses_address_book(&$vars) {
  // Initialize variables
  $vars['options'] += array(
    'add_link' => FALSE,
  );
  $vars['add_address_link'] = NULL;
  $vars['other_addresses'] = array();
  $vars['default_billing_address'] = NULL;
  $vars['default_shipping_address'] = NULL;

  // Find out which addresses are default addresses and which are not.
  $address_book = $vars['address_book'];
  foreach ($vars['addresses'] as $aid => $themed_address) {
    if ($address_object = $address_book->getAddressById($aid)) {
      $vars['other_addresses'][$aid] = $themed_address;
      if ($address_object->isDefault('billing') && variable_get('uc_addresses_use_default_billing', TRUE)) {
        $vars['default_billing_address'] = $themed_address;
        unset($vars['other_addresses'][$aid]);
      }
      if ($address_object->isDefault('shipping') && variable_get('uc_addresses_use_default_shipping', TRUE)) {
        $vars['default_shipping_address'] = $themed_address;
        unset($vars['other_addresses'][$aid]);
      }
    }
  }

  // Setup odd/even for the other addresses
  $count = 0;
  foreach ($vars['other_addresses'] as $num => $row) {
    $vars['row_classes'][$num][] = ($count++ % 2 == 0) ? 'odd' : 'even';
  }

  // Setup admin links
  if ($vars['options']['add_link']) {
    $vars['add_address_link'] = l(t('Add a new address'), 'user/' . $address_book->getUserId() . '/addresses/add', array('attributes' => array('class' => 'address-link add-address-link')));
  }
}

/**
 * Display a single address.
 *
 * If the current user can edit the address, then provide an edit
 * link.
 *
 * @param object $address_user
 *   The user who "owns" the address.
 * @param UcAddressesAddress $address
 *   The address to display.
 * @param array $options
 *   Options for display, see template_preprocess_uc_addresses_list_address().
 *
 * @return string
 *   The themed address.
 */
function uc_addresses_list_one_address($address_user, UcAddressesAddress $address, $options = array()) {
  drupal_add_css(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.css');

  if (UcAddressesPermissions::canViewAddress($address_user, $address)) {
    // Check if address may be edited too.
    if (UcAddressesPermissions::canEditAddress($address_user, $address)) {
      // Show edit link
      $options['edit_link'] = TRUE;
    }
    if (UcAddressesPermissions::canDeleteAddress($address_user, $address)) {
      // Show delete link
      $options['delete_link'] = TRUE;
    }
    return theme('uc_addresses_list_address', $address, $options);
  }
}

/**
 * Prepare variables for one address
 *
 * @param array $vars
 *   An array with the following:
 *   - address (instance of UcAddressesAddress)
 *   - options (array):
 *     - edit_link (boolean): whether or not display an edit address link.
 *     - delete_link (boolean): whether or not display an delete address link.
 *     - destination (string): path to return to after editing the address.
 *
 * @return void
 *
 * @ingroup themeable
 */
function template_preprocess_uc_addresses_list_address(&$vars) {
  // Initialize variables
  $vars['options'] += array(
    'edit_link' => FALSE,
    'delete_link' => FALSE,
    'destination' => '',
  );
  $vars['admin_links'] = NULL;

  // User ID
  $vars['aid'] = $vars['address']->getId();
  $vars['uid'] = $vars['address']->getUserId();

  // Preprocess address fields, context is 'address_view' in this case.
  $vars['fields'] = uc_addresses_preprocess_address($vars['address'], 'address_view');

  // Setup admin links
  if (isset($vars['options']['edit_link']) && $vars['options']['edit_link']) {
    $link_options = array(
      'attributes' => array('class' => 'address-link edit-address-link'),
    );
    if (isset($vars['options']['destination']) && $vars['options']['destination']) {
      $link_options['query'] = array('destination' => $vars['options']['destination']);
    }
    $vars['edit_address_link'] = l(t('Edit address'), 'user/' . $vars['uid'] . '/addresses/' . $vars['aid'] . '/edit', $link_options);
    $vars['admin_links'] = $vars['edit_address_link'];
  }
  if (isset($vars['options']['delete_link']) && $vars['options']['delete_link']) {
    $link_options = array(
      'attributes' => array('class' => 'address-link delete-address-link'),
    );
    if (isset($vars['options']['destination']) && $vars['options']['destination']) {
      $link_options['query'] = array('destination' => $vars['options']['destination']);
    }
    $vars['delete_address_link'] = l(t('Delete address'), 'user/' . $vars['uid'] . '/addresses/' . $vars['aid'] . '/delete', $link_options);
    if (isset($vars['options']['edit_link']) && $vars['options']['edit_link']) {
      $vars['admin_links'] .= ' <span class="separator">|</span> ' . $vars['delete_address_link'];
    }
    else {
      $vars['admin_links'] = $vars['delete_address_link'];
    }
  }

  // CSS classes
  $vars['classes_array'] = array();
  if (isset($vars['default_shipping']) && $vars['default_shipping']) {
    $vars['classes_array'][] = 'addresses-default-address';
    $vars['classes_array'][] = 'addresses-default-shipping-address';
  }
  if (isset($vars['default_billing']) && $vars['default_billing']) {
    $vars['classes_array'][] = 'addresses-default-address';
    $vars['classes_array'][] = 'addresses-default-billing-address';
  }
  $vars['classes'] = implode(' ', $vars['classes_array']);
}

// -----------------------------------------------------------------------------
// ADD/EDIT ADDRESSES
// -----------------------------------------------------------------------------

/**
 * Create a form used to add a new address or edit an existing address.
 *
 * @param array $form_state
 *   The form state.
 * @param object $address_user
 *   The user who "owns" this address.
 * @param UcAddressesAddress $address
 *   The address to edit (NULL for new addresses)
 *
 * @return array
 *   An address form
 *
 * @ingroup forms
 */
function uc_addresses_get_address_form(&$form_state, $address_user, $address = NULL) {
  if (!($address instanceof UcAddressesAddress)) {
    $address = UcAddressesAddressBook::get($address_user)->addAddress();
  }

  $form['uc_addresses'] = array(
    '#type' => 'fieldset',
    '#title' => ($address->isNew()) ? t('Add an address') : t('Edit address information'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['uc_addresses']['address'] = array(
    '#type' => 'uc_addresses_address',
    '#uc_addresses_address' => $address,
    '#uc_addresses_context' => 'address_form',
  );

  // Add submit button
  if ($address->isNew()) {
    // New address
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => variable_get('uc_addresses_add_button', t('Save address')),
    );
  }
  else {
    // Existing address
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => variable_get('uc_addresses_update_button', t('Save address')),
    );
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => variable_get('uc_addresses_delete_button', t('Delete address')),
    );

    // Check if address may be deleted
    // (It's not allowed to delete default addresses)
    if (!UcAddressesPermissions::canDeleteAddress($address_user, $address)) {
      $form['delete']['#access'] = FALSE;
    }
  }

  // Add cancel link
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'user/' . $address_user->uid . '/addresses/', array('attributes' => array('class' => 'address-link cancel-address-link'))),
  );

  return $form;
}

/**
 * Validate handler for the address form.
 *
 * Checks if the address is similar to other addresses.
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 *
 * @see uc_addresses_get_address_form()
 * @see uc_addresses_get_address_form_submit()
 */
function uc_addresses_get_address_form_validate(&$form, &$form_state) {
  switch ($form_state['clicked_button']['#value']) {
    case variable_get('uc_addresses_delete_button', t('Delete address')):
      // Don't do further validation.
      unset($form['uc_addresses']['address']['#element_validate']);
      unset($_SESSION['messages']['error']);
      $form_state['submitted'] = TRUE;
      form_set_error(NULL, '', TRUE);
      return;
  }
  // Check if the address looks like other addresses.
  try {
    $address = $form['uc_addresses']['address']['#uc_addresses_address'];
    $address_book = UcAddressesAddressBook::get($address->getUserId());
    if ($address_book->compareAddress($address)) {
      if ($address->isNew()) {
        form_set_error('uc_addresses][address', t('This address already appears in your address book. A new address was not added.'));
      }
      else {
        form_set_error('uc_addresses][address', t('The revised address is already in your address book. Your change was not made.'));
      }
    }
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
  }
}

/**
 * Submit handler for the address form.
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 *
 * @see uc_addresses_get_address_form()
 * @see uc_addresses_get_address_form_validate()
 */
function uc_addresses_get_address_form_submit(&$form, &$form_state) {
  try {
    $address = $form['uc_addresses']['address']['#uc_addresses_address'];

    if ($form_state['clicked_button']['#value'] == variable_get('uc_addresses_delete_button', t('Delete address'))) {
      unset($form_state['storage']); // Storage must be cleared or else the redirect does not work.
      $form_state['redirect'] = 'user/' . $address->getUserId() . '/addresses/' . $address->getId() . '/delete';
    }
    elseif ($form_state['clicked_button']['#value']) {
      $address->getAddressBook()->save();
      drupal_set_message(t('The address is saved.'));
      unset($form_state['storage']); // Storage must be cleared or else the redirect does not work.
      $form_state['redirect'] = 'user/' . $address->getUserId() . '/addresses';
    }
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
  }
}

/**
 * Theme the add or edit address form.
 *
 * @param array $form
 *   The form array to theme.
 *
 * @return string
 *   The themed form.
 *
 * @ingroup themeable
 */
function theme_uc_addresses_get_address_form($form) {
  drupal_add_css(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.css');
  return drupal_render($form);
}

/**
 * Prepare variables for address form
 *
 * @param array $vars
 *
 * @return void
 *
 * @ingroup themeable
 */
function template_preprocess_uc_addresses_form(&$vars) {
  drupal_add_css(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.css');
  $vars['req'] = '<span class="form-required">*</span>';
}

// -----------------------------------------------------------------------------
// DELETE ADDRESSES
// -----------------------------------------------------------------------------

/**
 * Display a confirmation page before deleting an address.
 *
 * @param object $address_user
 *   The user who "owns" the address.
 * @param UcAddressesAddress $address
 *   The address to delete.
 *
 * @return string
 */
function uc_addresses_delete_address_confirm($address_user, UcAddressesAddress $address) {
  $uid = $address_user->uid;
  $aid = $address->getId();

  $form = drupal_get_form('uc_addresses_delete_address_confirm_form', $address_user, $address);

  $help =
    variable_get('uc_addresses_delete_instructions',
      t('Are you are sure you want to delete this address? Once deleted, the address cannot be recovered.',
        array(
          '!delete' => variable_get('uc_addresses_delete_button', t('Delete address')),
        )
      )
  );

  $output = theme('uc_addresses_address_delete_confirm', $help, $form);

  return $output;
}

/**
 * Theme the address deletion confirmation form.
 *
 * @param string $help
 *   The help message to display.
 * @param string $form
 *   The HTML version of the form that by default
 *	 includes the 'Back' and 'Delete Address' buttons at the bottom
 *   of the confirmation page.
 *
 * @return string
 *   The themed confirmation form.
 *
 * @ingroup themeable
 * @todo Move to template?
 */
function theme_uc_addresses_address_delete_confirm($help, $form) {
  drupal_add_css(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.css');

  $output = '<p>' . $help . '</p>';
  $output .= $form;
  return $output;
}

/**
 * Get the submit buttons to confirm deletion of a user's address.
 *
 * @param array $form_state
 *   The state of the form.
 * @param object $user
 *   The user who "owns" the address.
 * @param UcAddressesAddress $address
 *   The address we are deleting.
 *
 * @return string
 *   The buttons for the form (as a string).
 *
 * @ingroup forms
 */
function uc_addresses_delete_address_confirm_form(&$form_state, $address_user, UcAddressesAddress $address) {
  $form['stored_values'] = array(
    '#type' => 'value',
    '#value' => array(
      'user' => $address_user,
      'address' => $address,
    ),
  );

  return confirm_form(
    $form,
    t('Delete address'),
      'user/' . $address_user->uid . '/addresses/',
    (string) $address,
    variable_get('uc_addresses_delete_button', t('Delete address')),
    t('Cancel')
  );
}

/**
 * Delete a user-confirmed address.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 *
 * @return void
 *
 * @see uc_addresses_delete_address_confirm_form()
 */
function uc_addresses_delete_address_confirm_form_submit($form, &$form_state) {
  $address_user = $form['stored_values']['#value']['user'];
  $address = $form['stored_values']['#value']['address'];

  try {
    if ($address->delete()) {
      drupal_set_message(t('The address has been deleted.'));
    }
    else {
      drupal_set_message(t('The address could not be deleted.'), 'error');
    }
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
  }

  $form_state['redirect'] = array('user/' . $address_user->uid . '/addresses');
}
