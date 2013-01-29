<?php
// $Id$

class views_customfield_calc_table extends views_calc_table {
  function render() {
    if (!empty($this->view->totals)) {
      $totals =& $this->view->totals;
      $i = 0;
      foreach ($this->get_calc_fields() as $calc => $fields) {
        foreach ($fields as $field) {
          $view_field = $this->view->field[$field];
          if ($view_field->table == 'customfield') {
            $total = $this->customfield_totals($view_field, $calc);
            $totals[$i]->{$view_field->field_alias} = $total;
          }
        }
        $i++;
      }
    }
    return parent::render();
  }

  function customfield_totals($field, $calc) {
    static $totals = array();
    if (!isset($totals[$field->field_alias])) {
      $totals[$field->field_alias] = array(
        'SUM' => 0.0,
        'COUNT' => count($this->view->result),
        'MIN' => $this->view->result[0]->{$field->field_alias}, 
        'MAX' => $this->view->result[0]->{$field->field_alias},
        'AVG' => 0.0,
      );
      if (!empty($this->view->result)) {
        foreach ($this->view->result as $row) {
          if (!isset($row->{$field->field_alias})) {
            $totals[$field->field_alias]['COUNT']--;
          }
          else {
            $value = $row->{$field->field_alias};
            $totals[$field->field_alias]['SUM'] += $value;
            if ($totals[$field->field_alias]['MAX'] < $value) $totals[$field->field_alias]['MAX'] = $value;
            if ($totals[$field->field_alias]['MIN'] > $value) $totals[$field->field_alias]['MIN'] = $value;
          }
        }
        $totals[$field->field_alias]['AVG'] = $totals[$field->field_alias]['SUM'] / $totals[$field->field_alias]['COUNT'];
      }
    }
    return $totals[$field->field_alias][$calc];
  }

  function query_sub_total() {
    // Create summary rows.
    $calc_fields = $this->get_calc_fields();
    $calc = $this->view->views_calc_calculation;
    $fields = $calc_fields[$calc];
    
    // Empty out any fields that have been added to the query,
    // we don't need them for the summary totals.
    $this->view->query->fields = array();
    foreach ($this->view->field as $field) {
      if ($field->table == 'customfield') continue;

      $query_field = substr($field->field, 0, 3) == 'cid' ? $field->definition['calc'] : $field->table .'.'. $field->field;
      $query_alias = $field->field_alias;
      if (in_array($field->field, $fields)) {
        // Calculated fields.
        $this->view->query->add_field(NULL, "$calc($query_field)", $query_alias);
      }
      else {
        // Empty fields that have no calculations.
        $this->view->query->add_field(NULL, "MAX('')", $query_alias);
      }
      // Add a dummy field for the groupby.
      $this->view->query->add_field(NULL, "MAX('". $calc ."')", "TOTAL_". $calc);
    }
    // TODO This won't work right with relationships, need a fix here.
    if (!empty($this->view->views_calc_nids)) {
      $this->view->query->add_where(NULL, "node.nid IN (%s)", implode(',', $this->view->views_calc_nids));
    }
  }
  
  function query_total() {
    // Create summary rows.
    $calc_fields = $this->get_calc_fields();
    $calc = $this->view->views_calc_calculation;
    $fields = $calc_fields[$calc];
    
    // Empty out any fields that have been added to the query,
    // we don't need them for the summary totals.
    $this->view->query->fields = array();
    // Clear out any sorting and grouping, it can create unexpected results
    // when Views adds aggregation values for the sorts.
    $this->view->query->orderby = array();
    $this->view->query->groupby = array();

    foreach ($this->view->field as $field) {
      if ($field->table == 'customfield') continue;

      $query_field = substr($field->field, 0, 3) == 'cid' ? $field->definition['calc'] : $field->table .'.'. $field->field;
      $query_alias = $field->field_alias;
      if (!empty($fields) && in_array($field->field, $fields)) {
        // Calculated fields.
        $this->view->query->add_field(NULL, "$calc($query_field)", $query_alias);
      }
      else {
        // Empty fields that have no calculations.
        $this->view->query->add_field(NULL, "MAX('')", $query_alias);
      }
      // Add a dummy field for the groupby.
      $this->view->query->add_field(NULL, "MAX('". $calc ."')", "TOTAL_". $calc);
    }
  }
}

