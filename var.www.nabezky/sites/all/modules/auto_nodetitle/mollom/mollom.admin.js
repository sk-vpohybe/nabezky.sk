(function ($) {

/**
 * Filters blacklist entries.
 */
Drupal.behaviors.mollomBlacklistFilter = function (context) {
  // Intentionally wrong indentation for simplified maintenance.
    var self = {};
    $('#mollom-blacklist:not(.mollom-processed)', context).addClass('mollom-processed').each(function () {
      // Prepare a list of all entries to optimize performance. Each key is a
      // blacklisted value and each value is an object containing the
      // corresponding table row, context, and match.
      self.entries = {};
      $(this).find('tr:has(.mollom-blacklist-value)').each(function () {
        var $row = $(this);
        self.entries[$row.find('.mollom-blacklist-value').text()] = {
          context: $row.children('.mollom-blacklist-context').attr('class').match(/value-(\w+)/)[1],
          match: $row.children('.mollom-blacklist-match').attr('class').match(/value-(\w+)/)[1],
          row: $row.get(0)
        };
      });

      // Attach the instant text filtering behavior.
      var $filterText = $('#mollom-blacklist-filter-value', context);
      var $filterContext = $('#mollom-blacklist-filter-context', context);
      var $filterMatch = $('#mollom-blacklist-filter-match', context);

      self.lastSearch = {};
      var filterRows = function () {
        // Prepare static variables and conditions only once.
        var i, value, visible, changed;
        var search = {
          // Blacklist entries are stored in lowercase, so to get any filter
          // results, the entered text must be converted to lowercase, too.
          value: $filterText.val().toLowerCase(),
          context: $filterContext.val(),
          match: $filterMatch.val()
        };
        // Immediately cancel processing if search values did not change.
        changed = false;
        for (i in search) {
          if (search[i] != self.lastSearch[i]) {
            changed = true;
            break;
          }
        }
        if (!changed) {
          return;
        }
        self.lastSearch = search;
        // Blacklists can contain thousands of entries, so we use a simple
        // for...in loop instead of jQuery.each() to save many function calls.
        // Likewise, we directly apply the 'display' style, since
        // jQuery.fn.hide() and jQuery.fn.show() call into jQuery.fn.animate(),
        // which is useless for this purpose.
        for (value in self.entries) {
          visible = (search.value.length == 0 || value.indexOf(search.value) != -1);
          visible = visible && (search.context.length == 0 || self.entries[value].context == search.context);
          visible = visible && (search.match.length == 0 || self.entries[value].match == search.match);
          if (visible) {
            self.entries[value].row.style.display = '';
          }
          else {
            self.entries[value].row.style.display = 'none';
          }
        }
      };
      $filterText.bind('keyup change', filterRows);
      $filterContext.change(filterRows);
      $filterMatch.change(filterRows);
    });
};

})(jQuery);
