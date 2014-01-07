<?php

DEFINE('GTILESIZE', 256);
DEFINE('DEGREES_PADDING', 0.1);
/* Where we will store our cached graphs */
DEFINE('TRACKFIELD_MAP_STATIC_CACHE_PATH', 'trackfield_cache/static_map');
/* Where we will store cached tiles */
DEFINE('TRACKFIELD_MAP_STATIC_TILE_CACHE_DIR', 'trackfield_cache/static/tiles');

require_once('trackfield_graph.inc');
require_once('trackfield_map_projection.inc');
require_once('gmaptile.php');

function _trackfield_map_static_do_map() {
	/*
	 * Re-use a load of the graph functions.
	 * TODO: Clean this up
	 */

	$args = _trackfield_graph_graphargs();
	$trackfield_map_format_sizes = variable_get('trackfield_map_format_sizes', array());
  	$format_config = $trackfield_map_format_sizes[$args['formatter']];
  	if (!$format_config) {
  		/* Invalid formatter */
  		return drupal_not_found();
  	} else {
		$modified = filemtime(__FILE__); /* TODO max() with $config['timestamp'] */
		$modified = _trackfield_graph_check_modified($args, $modified);

		/*
		 * We have to return an image for this request - see if we have this
		 * in our cache.
		 */
		$cache_file = _trackfield_graph_cache_file(TRACKFIELD_MAP_STATIC_CACHE_PATH, $args);
		if (!file_exists($cache_file) ||
			filemtime($cache_file) < $modified) {
			_trackfield_map_static_create_cache_file($cache_file, $args, $format_config);
		}
	
		$headers = array(
			'Content-Type: image/png',
			'Content-Length: ' . filesize($cache_file),
			'Last-Modified: ' . gmdate('D, d M Y H:i:s', $modified) . ' GMT',
			'Expires: '. gmdate('D, d M Y H:i:s', time() + TRACKFIELD_EXPIRES_OFFSET) .' GMT',
			'Cache-Control: public, must-revalidate, max-age=0');
		file_transfer($cache_file, $headers);
  	}
}
			
function _trackfield_map_static_create_cache_file($cache_file, $args, $format_config) {			
	$width = $format_config['width'];
	$height= $format_config['height'];
	$thickness = 2;
	
	$sql = <<<EOF
SELECT settype, setdata
FROM {trackfield_datasets} td
WHERE td.nid = %d
AND td.vid = %d
AND td.field_name = '%s'
AND td.delta = %d
AND settype = '%s'
EOF;
		
	$trackset = array();
	foreach ($args['items'] as $item) {
		$oneset = array();
		foreach (array('latitude', 'longitude') as $settype) {
			$result = db_query($sql, $item['nid'], $item['vid'], $item['field'], $item['delta'], $settype);
			if ($row = db_fetch_object($result)) {
				$oneset[$row->settype] = split(',', $row->setdata);
			}
		}
		$trackset[] = $oneset;
	}

	/*
	 * Get bounds for these tracks & tile area needed.
	 */

	$bounds = array();	 
	foreach ($trackset as $oneset) {
		$bounds = _trackfield_map_merge_bounds($bounds, _trackfield_map_find_bounds($oneset, DEGREES_PADDING));
	} 
	list($zoom, $nw, $se) = _trackfield_map_static_tiles($bounds, $width, $height);
	
	/*
	 * Now adjust the bounds to make the output image aspect ration match width/height...
	 */

	$bounds = _trackfield_map_static_adjust_bounds($bounds, $width, $height, $nw, $zoom);
	list($zoom, $nw, $se) = _trackfield_map_static_tiles($bounds, $width, $height);
		
	/*
	 * ... and make sure the tiles cover this new area.
	 */
	$tile_img = _trackfield_map_static_tiled_image($zoom, $nw, $se);
	
	/*
	 * Re-size image prior to line drawing, so line thickness is consistant.
	 */
	$proj = new trackfield_map_projection($nw, $zoom);
	$minx = $proj->toXpixel($bounds[1]);
	$miny = $proj->toYpixel($bounds[2]);
	$maxx = $proj->toXpixel($bounds[3]);
	$maxy = $proj->toYpixel($bounds[0]);

	$out_img = imagecreatetruecolor($width, $height);
	imagecopyresampled($out_img, $tile_img, 0, 0, $minx, $miny, $width, $height, ($maxx - $minx), ($maxy - $miny));
	
	/*
	 * Adjust projection
	 */
	$scale = $width / ($maxx - $minx);
	$proj->adjustSize($width / ($maxx - $minx));
	$proj->setXoffset(-$minx * $scale);
	$proj->setYoffset(-$miny * $scale);
	
	$colour = imagecolorallocate($out_img, 255, 0, 0);
	imagesetthickness($out_img, $thickness);
	
	foreach ($trackset as $oneset) {
		$last_point = $proj->toXYpixel($oneset['longitude'][0], $oneset['latitude'][0]);
		for ($lp = 1; $lp < count($oneset['latitude']); $lp++) {
			$point = $proj->toXYpixel($oneset['longitude'][$lp], $oneset['latitude'][$lp]);
			imageline($out_img, $last_point[0], $last_point[1], $point[0], $point[1], $colour);
	    	$last_point = $point;
		}
	}

	imagepng($out_img, $cache_file);
}

function _trackfield_map_static_get_tile_img($x, $y, $z) {
	$tile_url = _trackfield_map_static_tile_url($x, $y, $z);
	
	/*
	 * MD5 gives 32 chars... split into 4 char chunks. It's not like we'll
	 * store every possible combination, but this will make the directory
	 * sizes smaller.
	 */
		 
	$md5_parts = str_split(md5($tile_url), 2);
	$cache_filename = array_pop($md5_parts);
	$cache_pathname = file_directory_path() . '/' . TRACKFIELD_MAP_STATIC_TILE_CACHE_DIR . '/' . join('/', $md5_parts);

	$cache_file = $cache_pathname. '/' . $cache_filename;
	/* Make sure the directory exists - file_create_path will fail if not */
	if (!is_dir($cache_pathname)) {
		mkdir($cache_pathname, 0755, true);
	}

	if (!file_exists($cache_file)) {
		if ($file = fopen($tile_url, 'r')) {
			$img = stream_get_contents($file, -1);
			fclose($file);
		}
		if ($img) {
			if ($file = fopen($cache_file, 'w')) {
				fwrite($file, $img);
				fclose($file);
			}
		}
	}

	return imagecreatefromjpeg($cache_file);
}

function _trackfield_map_static_tile_url($x, $y, $z) {
	/* TODO: make this a parameter */
	return sprintf('http://services.arcgisonline.com/ArcGIS/rest/services/World_Topo_Map/MapServer/tile/%d/%d/%d', $z, $y, $x);
}

/*
 * Adjust the lat/lon bounds to fit the aspect ratio given.
 */

function _trackfield_map_static_adjust_bounds($bounds, $width, $height, $nw, $zoom) {
	$aspect = $width / $height;
	
	$proj = new trackfield_map_projection($nw, $zoom);
	$minx = $proj->toXpixel($bounds[1]);
	$miny = $proj->toYpixel($bounds[2]);
	$maxx = $proj->toXpixel($bounds[3]);
	$maxy = $proj->toYpixel($bounds[0]);

	$bwidth = $maxx - $minx;
	$bheight = $maxy - $miny;
	$baspect = $bwidth / $bheight;
	
	if ($baspect > $aspect) {
		/* Add pixels top & bottom */
		$pixels = (($bwidth / $aspect) - $bheight) / 2 ;
		$miny -= $pixels;
		$maxy += $pixels; 
	} else if ($baspect < $aspect) {
		/* Add pixels left & right */
		$pixels = (($bheight * $aspect) - $bwidth) / 2 ;
		$minx -= $pixels;
		$maxx += $pixels; 
	}
	$bwidth = $maxx - $minx;
	$bheight = $maxy - $miny;
	$baspect = $bwidth / $bheight;
	
	return array($proj->toLat($maxy), $proj->toLon($minx), $proj->toLat($miny), $proj->toLon($maxx));
}

/*
 * Figure out which tiles will be needed for this area.
 * Returns zoom, top left/bottom right corners. 
 */

function _trackfield_map_static_tiles($bounds, $width, $height) {
	$max_tiles = ceil(max($width, $height) / GTILESIZE) + 1;
	$zoom = 18;
	do {
		$zoom--;
		$nw = new GMapTile($bounds[2], $bounds[1], $zoom);
		$se = new GMapTile($bounds[0], $bounds[3], $zoom);
		
		$xtiles = $se->getTileCoord()->x - $nw->getTileCoord()->x + 1;
		$ytiles = $se->getTileCoord()->y - $nw->getTileCoord()->y + 1;
	} while (max($xtiles, $ytiles) > $max_tiles);
	
	return array($zoom, $nw, $se);
}

/*
 * Generate an image made by merging required tiles.
 */

function _trackfield_map_static_tiled_image($zoom, $nw, $se) {
	$tiles_wide = $se->getTileCoord()->x - $nw->getTileCoord()->x + 1;
	$merge_width = $tiles_wide * GTILESIZE;
	$tiles_high = $se->getTileCoord()->y - $nw->getTileCoord()->y + 1;
	$merge_height = $tiles_high * GTILESIZE;

	$out_img = imagecreatetruecolor($merge_width, $merge_height);
	
	$x = $nw->getTileCoord()->x;
	$y = $nw->getTileCoord()->y;
	
	$merge_y = 0;
	for ($y = $nw->getTileCoord()->y; $y <= $se->getTileCoord()->y; $y++) {
		$merge_x = 0;
		for ($x = $nw->getTileCoord()->x; $x <= $se->getTileCoord()->x; $x++) {
			$tile_img = _trackfield_map_static_get_tile_img($x, $y, $zoom);
			imagecopy($out_img, $tile_img, $merge_x, $merge_y, 0, 0, GTILESIZE, GTILESIZE);
			$merge_x += GTILESIZE;
		}
		$merge_y += GTILESIZE;
	}
	
	return $out_img;
}
