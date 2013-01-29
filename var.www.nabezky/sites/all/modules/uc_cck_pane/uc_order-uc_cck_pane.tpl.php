<?php

/**
 * @file
 * This file provides a snippet of template code that you can insert into your invoice template to display CCK pane values.
 */
?>

<!-- CCK Panes -->
<?php
  $types = uc_cck_pane_get_types('types');
  foreach ($types as $type) {
    $node_type = content_types($type->type);
?>
    <tr>
      <td colspan="2" bgcolor="#006699">
        <b><?php echo t($type->name); ?></b>
      </td>
    </tr>
    <tr>
      <td colspan="2">

        <table border="0" cellpadding="1" cellspacing="0" width="100%" style="font-family: verdana, arial, helvetica; font-size: small;">
          <?php
            if ($node_type['has_title']) {
          ?>
              <tr>  
                <td nowrap="nowrap">
                  <b><?php echo t($node_type['title_label']); ?>:</b>
                </td>
                <td width="98%">
                  <?php echo ${'uc_cck_'.$type->type.'_title'}; ?>
                </td>
              </tr>
          <?php
            }
            if ($node_type['has_body']) {
          ?>
              <tr>
                <td nowrap="nowrap">
                  <b><?php echo t($node_type['body_label']); ?>:</b>
                </td>
                <td width="98%">
                  <?php echo ${'uc_cck_'.$type->type.'_body'}; ?>
                </td>
              </tr>
          <?php
            }
            foreach ($node_type['fields'] as $fid => $field) {
          ?>
              <tr>
                <td nowrap="nowrap">
                  <b><?php echo t($field['widget']['label']); ?>:</b>
                </td>
                <td width="98%">
                  <?php echo ${'uc_cck_'.$type->type.'_'.$fid}; ?>
                </td>
              </tr>  
          <?php
            }
          ?>
        </table>
      </td>
    </tr>
<?php
  }
?>
<!-- End CCK Panes -->
