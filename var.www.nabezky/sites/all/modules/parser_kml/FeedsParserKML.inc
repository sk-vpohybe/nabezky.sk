<?php
// $Id: FeedsParserKML.inc,v 1.1.2.1 2011/02/09 18:26:57 jcnventura Exp $

/**
 * @file
 *
 * Provides the HTML and XML Classes for Feeds KML Parser.
 */

/**
 * Base class for the HTML and XML parsers.
 */
class FeedsParserKML extends FeedsParser {
  /**  
   * Implementation of FeedsParser::parse().  
   */
  public function parse(FeedsImportBatch $batch, FeedsSource $source) {
    $raw = trim($batch->getRaw());  

    if (empty($raw)) {
      drupal_set_message(t('Feeds GPX parser: The XML document is empty.'), 'warning');
      return;
    }

    $xml = new SimpleXMLElement($raw);
    $xml_ns = $xml->getNamespaces();
    $xml->registerXPathNamespace('k', current($xml_ns));
    foreach ($xml->xpath('//k:trkpt') as $trkpt) {
        $item = array(
          'name' => (string) $trkpt->name,
          'description' => (string) $trkpt->desc,
          'longitude' => (string) $trkpt['lon'],
          'latitude' => (string) $trkpt['lat'],
          'datetime' => (string) $trkpt->time,
        );
        
        if (module_exists('openlayers_cck')) {
          $item['openlayers_wkt'] = "GEOMETRYCOLLECTION(POINT(${trkpt['lon']} ${trkpt['lat']}))";
        }

        $batch->items[] = $item;
    }
  }

  /**
   * Return mapping sources.
   *
   * At a future point, we could expose data type information here,
   * storage systems like Data module could use this information to store
   * parsed data automatically in fields with a correct field type.
   */
  public function getMappingSources() {
    $sources = array(
      'name' => array(
        'name' => t('Name'),
        'description' => t('Location name.'),
      ),
      'description' => array(
        'name' => t('Description'),
        'description' => t('Location description.'),
      ),
      'longitude' => array(
        'name' => t('Longitude'),
        'description' => t('Longitude coordinates of the location in text format.'),
      ),
      'latitude' => array(
        'name' => t('Latitude'),
        'description' => t('Latitude coordinates of the location in text format.'),
      ),
      'datetime' => array(
        'name' => t('Datetime'),
        'description' => t('Date and time of when the coordinates were recorded.'),
      ),
    );
    if (module_exists('openlayers_cck')) {
      $sources['openlayers_wkt'] = array(
        'name' => t('OpenLayers WKT'),
        'description' => t('Location coordinates in OpenLayers WKT format.'),
      );
    }
    return $sources + parent::getMappingSources();
  }
}
