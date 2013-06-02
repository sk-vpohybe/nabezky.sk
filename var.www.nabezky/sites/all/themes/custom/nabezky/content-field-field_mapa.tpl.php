<?php
// $Id: content-field.tpl.php,v 1.1.2.6 2009/09/11 09:20:37 markuspetrux Exp $

/**
 * @file content-field.tpl.php
 * Default theme implementation to display the value of a field.
 *
 * Available variables:
 * - $node: The node object.
 * - $field: The field array.
 * - $items: An array of values for each item in the field array.
 * - $teaser: Whether this is displayed as a teaser.
 * - $page: Whether this is displayed as a page.
 * - $field_name: The field name.
 * - $field_type: The field type.
 * - $field_name_css: The css-compatible field name.
 * - $field_type_css: The css-compatible field type.
 * - $label: The item label.
 * - $label_display: Position of label display, inline, above, or hidden.
 * - $field_empty: Whether the field has any valid value.
 *
 * Each $item in $items contains:
 * - 'view' - the themed view for that item
 *
 * @see template_preprocess_content_field()
 */
?>
<?php if (!$field_empty) : ?>
<div class="field field-type-<?php print $field_type_css ?> field-<?php print $field_name_css ?>">
  <?php if ($label_display == 'above') : ?>
    <div class="field-label"><?php print t($label) ?>:&nbsp;</div>
  <?php endif;?>
  <div class="field-items">
    <?php $count = 1;
    foreach ($items as $delta => $item) :
      if (!$item['empty']) : ?>
        <div class="field-item <?php print ($count % 2 ? 'odd' : 'even') ?>">
          <?php if ($label_display == 'inline') { ?>
            <div class="field-label-inline<?php print($delta ? '' : '-first')?>">
              <?php print t($label) ?>:&nbsp;</div>
          <?php } ?>
          <?php print $item['view'] ?>
        </div>
      <?php $count++;
      endif;
    endforeach;?>
  </div>
<div class="view-footer">
      Relief and Contour Map derived from <a href="http://www2.jpl.nasa.gov/srtm/">Shuttle Radar Topography Mission</a> data set. Copyright &copy;2013 various <a href="http://www.openstreetmap.org/">OpenStreetMap</a> contributors. Licensed as <a href="http://creativecommons.org/licenses/by-sa/2.0/">Creative Commons (CC-BY-SA 2.0)</a>. Some rights reserved. Copyright &copy;2007-2013 <a href="http://wiki.freemap.sk/About">Freemap Slovakia</a>
</div>
</div>
<?php endif; ?>
