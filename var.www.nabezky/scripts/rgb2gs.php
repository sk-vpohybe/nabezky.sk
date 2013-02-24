<?php

/**
 * This script converts uploaded rgb image into grayscale.
 */

$dirPath = '/var/www/nabezky/sites/default/files/ftp/';
$HTTPath = 'http://www.nabezky.sk/sites/default/files/ftp/';

$filesRev = glob($dirPath . '*.{jpg}', GLOB_BRACE); //returns array of file paths for jpg images with the oldest listed first
$files = array_reverse($filesRev);
$newest_file = end(explode("/", $files[0]));
$imagepath = $dirPath . $newest_file;
$fileName = basename($imagepath, ".jpg");
$serverGSimagePath = $dirPath . $fileName . '-' . rand(111111111, 999999999) . '.jpeg';
$path = $HTTPath . $newest_file;

if (is_readable($imagepath))
{
$image = imagecreatefromjpeg($path);
imagefilter($image, IMG_FILTER_GRAYSCALE);
imagejpeg($image, $serverGSimagePath, 100);
imagedestroy($image);
}
