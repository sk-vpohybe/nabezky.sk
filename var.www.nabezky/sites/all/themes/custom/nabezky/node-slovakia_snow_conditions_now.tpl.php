<?php
// $Id: node.tpl.php,v 1.5 2007/10/11 09:51:29 goba Exp $
?>
<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?>">

<?php print $picture ?>

	<?php if ($page == 0): ?>
	<h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
	<?php endif; ?>
<!--	
	<?php if ($submitted): ?>
	<span class="submitted"><?php print $submitted; ?></span>
	<?php endif; ?>
-->
<p>Mapa zobrazuje bežkárske trasy na Slovensku ktoré máme v databáze, ako aj aktuálnu informáciu o type stopy na týchto trasách. Ak chcete vidieť len trasy aktualizované za ostatných 24 hodín (alebo týždeň) tak stlačte tlačidlo "deň" (týždeň).</p>
<p>
	<strong>
		<span style="color:#ff0000;">Stav trasy môžte aktualizovať aj vy</span> hneď teraz ak ho poznáte. Stačí kliknúť na trasu a v informačnej bubline ktorá sa objaví stlačiť tlačidlo <span style="color:#ff0000;">"Aktualizovať"</span>
	</strong>
</p>
<table style="width: 65%; font-size:10px;">
	<tbody style="border-top: none;">
	<tr>
		<td style="padding: 0px;" rowspan="3" valign="top">
			<strong>Typ trasy</strong>
		</td>
		<td style="padding: 0px;">
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
		<td style="padding: 0px;">
			<div class="tracks-from-reports">
				<span style="margin-left: 30px;">trasy zo správ (stav sa nedá aktualizovať)</span>
			</div>
		</td>
	</tr>
	</tbody>
</table>
<div class="track-status-selector">Zobraziť trasy aktualizované za ostatný:
	<div class="status-one-day">
		<a href="/node/831/all/all/all/all/all/all/all/all?changed=-1+days">deň</a>
	</div>
	<div class="status-one-day">
		<a href="/node/831/all/all/all/all/all/all/all/all?changed=-1+week">týždeň</a>
	</div>
	<div class="status-all">
		<a href="/node/831/all/all/all/all/all/all/all/all?changed=-1000+years">všetky</a>
	</div>
</div>
  <div class="content clear-block">
    <?php print $content ?>
  </div>
  <div class="clear-block">
    <div class="meta">
    <?php if ($taxonomy): ?>
      <div class="terms"><?php print $terms ?></div>
    <?php endif;?>
    </div>

    <?php if ($links): ?>
      <div class="links"><?php print $links; ?></div>
    <?php endif; ?>
  </div>

</div>
