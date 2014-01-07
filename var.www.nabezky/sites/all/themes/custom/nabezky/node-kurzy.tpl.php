<div id="node">
	<div class="productnode">
		<div class="datum-price">
			<div class="datum">
				<strong><?php print $node->content['field_class_date']['field']['#title']; ?>:</strong>
				<?php print $node->field_class_date[0]['view'];  ?>
			</div>
			<div class="product-price">
				<?php print uc_currency_format($node->sell_price); ?></p>
			</div>
		</div>

		<div class="stredisko">
			<strong><?php print $node->content['field_ski_center']['field']['#title']; ?>:</strong>
			<div class="ski-centers">
				<?php
				// get all ski centers
				foreach ($node->field_ski_center as $sc) {
				?>
				<?php print $sc['view']; ?><br/>
				<?php   
				}
				?>
			</div>
		</div>

		<div class="trener">
			<strong><?php print $node->content['field_class_trainer']['field']['#title']; ?>:</strong>
			<?php print $node->field_class_trainer[0]['view']; ?>
		</div>
		
		<?php // product description  ?>
		<div class="productdesc">
			<?php print $node->content['body']['#value'];  ?>
		</div>


		<div id="cartButtons">
			<?php // add to cart buttons ?>
			<?php print $node->content['add_to_cart']["#value"]; ?>
		</div>
	</div>
</div>