--------------------------
Ubercart CCK Checkout Pane
--------------------------

Maintained by Martin B. - martin@webscio.net
Supported by JoyGroup - http://www.joygroup.nl


Introduction
------------
This module allows you to define CCK nodes as checkout panes in Ubercart.

This is useful if you want to collect additional information during the checkout process. For example, you may
want a quick survey or require additional fields that apply to the entire order.

Using the CCK module allows for many advantages: the forms/fields can easily be modified by an end-user; less
development time is required to create database tables and code to store/retrieve the data; and the form will
remember the user's previous answers.

Furthermore, this module now provides tokens for every field in each of your enabled content types, making it easy for
you to insert the submitted values into confirmation emails, invoices, etc.


Installation
------------
 * Copy the module's directory to your modules directory and activate the module.

Usage
-----
 * Every CCK content type on your site can be activated as a Ubercart checkout pane. In order to do so, go to the edit
 screen of the content type, then expand the 'Ubercart CCK Checkout Pane' fieldset and enable the 'Generate a
 checkout pane' option inside it.

 * The selected content types will now be listed under admin/store/settings/checkout/edit/panes where you can enable,
 disable and move around checkout panes. These act as normal Ubercart panes from here on in.

 * You can view/edit nodes relevant to the orders in your store directly through the orders, no need
 to go looking for the node anywhere else.

 * The module provides full token support for the content types acting as Ubercart checkout panes. To
 see a full list of available tokens, go to admin/store/help/tokens and look for those that look like
 'uc-cck-[typename]-[fieldname]'. Additionally, an example template file (uc_order-uc_cck_pane.tpl.php) is also
 provided by the module which shows how to dynamically add all tokens from all types to your invoice template.

Notes
-----
 * Fieldsets are not supported.

 * For Views integration see http://drupal.org/node/1157562.
