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
<p><?php
  print t('The map shows x-country trails in Slovakia as well as recent information about their track status. To display only trails with status updated within the last 24 hours (or week) press the "day" ("week") button.') ;
?></p>
<p>
	<strong>
		<span style="color:#ff0000;"><?php
  print t('You can update any trail status');
?></span><?php
  print t(' right now if you know it. Just click the trail path and in the info bubble that pops up press the '); ?><span style="color:#ff0000;"><?php print t('"Update"'); ?></span><?php
  print t(' button.') ; ?></strong>
</p>
<table style="width: 99%; font-size:10px; margin-top: 5px;">
	<tbody style="border-top: none;">
	<tr>
		<td style="padding: 0px;" rowspan="3" valign="top">
			<strong><?php
  print t('Track Type') ;
?></strong>
		</td>
		<td style="padding: 0px;">
			<div class="track-type-1">
				<span style="margin-left: 30px;"><?php
  print t('Machine Groomed - Free Style') ;
?></span>
			</div>
		</td>
		<td style="padding: 0px;">
			<div class="track-type-4">
				<span style="margin-left: 30px;"><?php
  print t('No Track') ;
?></span>
			</div>
		</td>
	</tr>
	<tr>
		<td style="padding: 0px;">
			<div class="track-type-2">
				<span style="margin-left: 30px;"><?php
  print t('Machine Groomed - Classic Style') ;
?></span>
			</div>
		</td>
		<td style="padding: 0px;">
			<div class="track-type-5">
				<span style="margin-left: 30px;"><?php
  print t('State Unknown') ;
?></span>
			</div>
		</td>
	</tr>
	<tr>
		<td style="padding: 0px;">
			<div class="track-type-3">
				<span style="margin-left: 30px;"><?php
  print t('Skier-blazed Track') ;
?></span>
			</div>
		</td>
		<td style="padding: 0px;">
			<div class="tracks-from-reports">
				<span style="margin-left: 30px;"><?php
  print t('Trails Reported by Skiers') ;
?></span>
			</div>
		</td>
	</tr>
	</tbody>
</table>
<div class="track-status-selector"><?php
  print t('Display Tracks Updated in the Last:') ;
?>
	<div class="status-one-day">
		<a href="?changed=-1+days"><?php
  print t('Day') ;
?></a>
	</div>
	<div class="status-one-day">
		<a href="?changed=-1+week"><?php
  print t('Week') ;
?></a>
	</div>
	<div class="status-all">
		<a href="?changed=-1000+years"><?php
  print t('All') ;
?></a>
	</div>
</div>
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
