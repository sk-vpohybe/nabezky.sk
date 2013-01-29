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
  <div class="content clear-block">
    <?php print $content ?>
  </div>
  
	<div class="clear-block">
		<?php
			$viewName = 'reports_in_region';
			$display_id = page;
			$path = drupal_get_path_alias($_GET['q']);
			$path = explode('/', $path);
			print views_embed_view($viewName, $display_id,$path[1]);
		?>
	</div>
	<div class="clear-block" style="margin-top: 35px;">
		<?php
			$viewName = 'xctrails';
			$display_id = page_1;
			$path = drupal_get_path_alias($_GET['q']);
			$path = explode('/', $path);
			print views_embed_view($viewName, $display_id,$path[1]);
		?>
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
