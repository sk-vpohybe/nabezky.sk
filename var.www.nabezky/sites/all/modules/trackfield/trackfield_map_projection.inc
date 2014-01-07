<?php

class trackfield_map_projection {
	private $projectionsize, $projectionright, $projectiontop;
	private $nw, $zoom;
	private $xoffset, $yoffset;
	
	public function __construct($nw, $zoom) {
		$this->nw = $nw;
		$this->zoom = $zoom;

		$this->projectionsize = pow(2, $zoom) * GTILESIZE;
		$this->projectionright = ($this->nw->getTileCoord()->x) * GTILESIZE;
		$this->projectiontop = $this->projectionsize - $this->nw->getTileCoord()->y * GTILESIZE;
		
		$this->xoffset = 0;
		$this->yoffset = 0;
	}
	
	public function adjustSize($scale) {
		$this->projectionsize *= $scale;

		$this->projectionright = ($this->nw->getTileCoord()->x) * GTILESIZE * $scale;
		$this->projectiontop = $this->projectionsize - $this->nw->getTileCoord()->y * GTILESIZE * $scale;
	}
	
	public function setXoffset($offset) {
		$this->xoffset = $offset;
	}

	public function setYoffset($offset) {
		$this->yoffset = $offset;
	}

	public function toXpixel($lon) {
		return floor(($this->projectionsize / 360) * ($lon + 180) - $this->projectionright) + $this->xoffset;
	}
	
	public function toYpixel($lat) {
		return floor($this->projectiontop - ((atanh(sin(deg2rad($lat))) / M_PI) + 1) * $this->projectionsize / 2) + $this->yoffset;
	}

	public function toXYpixel($lon, $lat) {
		return array($this->toXpixel($lon), $this->toYpixel($lat));
	}

	public function toLon($x) {
		return ($x - $this->xoffset + $this->projectionright) / ($this->projectionsize / 360) - 180;
	}

	public function toLat($y) {
		return rad2deg(asin(tanh((((($this->projectiontop - $y - $this->yoffset) / $this->projectionsize * 2) - 1) * M_PI))));
	}
}