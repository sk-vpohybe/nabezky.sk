<?php
// $Id: node.tpl.php,v 1.5 2007/10/11 09:51:29 goba Exp $
?>
<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?>">

<?php print $picture ?>

	<?php if ($page == 0): ?>
	<h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
	<?php endif; ?>
	<table style="width:99.2%;">
		<tr style="background-color: #1659AC;">
			<td style="text-align:left; padding: 6px; padding-top: 2px; padding-bottom: 1px;">
				<a href="http://www.nabezky.sk/" target="_nb-editor"><img src="/sites/default/files/www_nabezky_logo.png" style="height:28px; padding-top:2px; border:none;" /></a>
			</td>
			<td style="text-align:right; color: gold; padding-right: 5px;">
				Trasy aktualizované za ostatný:
				<div class="status-one-day">
					<a href="<?php print arg(2); ?>?changed=-1+days">deň</a>
				</div>
				<div class="status-one-day">
					<a href="<?php print arg(2); ?>?changed=-1+week">týždeň</a>
				</div>
				<div class="status-all">
					<a href="<?php print arg(2); ?>?changed=-1000+years">všetky</a>
				</div>
			</td>
		</tr>
	</table>
  <div class="content clear-block" style="width:99%; border:solid thin black;">
    <?php print $content ?>
  </div>
<div class="attribution">Relief and Contour Map derived from <a href="http://www2.jpl.nasa.gov/srtm/">Shuttle Radar Topography Mission</a> data set. Copyright &copy;2012 various <a href="http://www.openstreetmap.org/">OpenStreetMap</a> contributors. Licensed as <a href="http://creativecommons.org/licenses/by-sa/2.0/">Creative Commons (CC-BY-SA 2.0)</a>. Some rights reserved. Copyright &copy;2007-2012 <a href="http://wiki.freemap.sk/About">Freemap Slovakia</a></div>
<table style="width: 99%; font-size:10px; margin-top: 5px;">
	<tbody style="border-top: none;">
	<tr>
		<td style="padding: 0px; width:60px;" rowspan="3" valign="top">
			<strong>Legenda</strong>
		</td>
		<td style="padding: 0px; width:250px;">
			<div class="track-type-1">
				<span style="margin-left: 30px;">strojom upravená pre voľný štýl</span>
			</div>
		</td>
		<td style="padding: 0px;">
			<div class="track-type-4">
				<span style="margin-left: 30px;">žiadna stopa</span>
			</div>
		</td>
	</tr>
	<tr>
		<td style="padding: 0px;">
			<div class="track-type-2">
				<span style="margin-left: 30px;">strojom upravená pre klasický štýl</span>
			</div>
		</td>
		<td style="padding: 0px;">
			<div class="track-type-5">
				<span style="margin-left: 30px;">stav neznámy</span>
			</div>
		</td>
	</tr>
	<tr>
		<td style="padding: 0px;">
			<div class="track-type-3">
				<span style="margin-left: 30px;">bežkármi prešliapaná</span>
			</div>
		</td>
		<td style="padding: 0px; font-weight: bold;">Pre viac info kliknite na trasu.</td>
	</tr>
	</tbody>
</table>
</div>
