const Gdk = imports.gi.Gdk;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const BOX_WIDTH_CHARS = 25;
const CELL_PADDING_X = 8;
const CELL_PADDING_Y = 6;

/**
 * Class: SearchBox
 *
 * This is a Search Box with autocompletion functionality.
 * The primary icon is a magnifying glass and the cursor turns into a hand when
 * hovering over the icon.
 *
 * NOTE: Due to a limitation in GTK, the cursor change will not work if the
 * search box's alignment is set to Gtk.Align.FILL in either direction.
 *
 */
const SearchBox = new Lang.Class({
    Name: 'SearchBox',
    GTypeName: 'EosSearchBox',
    Extends: Gtk.Entry,
    Signals: {
        /**
         * Event: menu-item-selected
         *
         * This event is triggered when an item is selected from the autocomplete menu.
         */
        'menu-item-selected': {
            param_types: [GObject.TYPE_STRING]
        },
        /**
         * Event: text-changed
         *
         * This event is triggered when the text in the search entry is changed by the user.
         */
        'text-changed': {
            param_types: [GObject.TYPE_STRING]
        }
    },

    _init: function(props) {
        this.parent(props);

        this.primary_icon_name = 'edit-find-symbolic';
        this.set_width_chars(BOX_WIDTH_CHARS);

        this._auto_complete = new Gtk.EntryCompletion();

        this._list_store = new Gtk.ListStore();
        this._list_store.set_column_types([GObject.TYPE_STRING]);

        this._auto_complete.set_model(this._list_store);
        this._auto_complete.set_text_column(0);

        let cells = this._auto_complete.get_cells();
        cells[0].xpad = CELL_PADDING_X;
        cells[0].ypad = CELL_PADDING_Y;

        this._auto_complete.set_match_func(function () { return true; });
        this.completion = this._auto_complete;

        this.connect('icon-press', Lang.bind(this, function () {
            this.emit('activate');
        }));
        this.completion.connect('match-selected', this._onMatchSelected.bind(this));
        this.connect('changed', Lang.bind(this, function () {
            if (!this._entry_changed_by_widget) {
                // If there is entry text, need to add the 'go' icon
                this.secondary_icon_name = (this.text.length > 0)? 'go-next-symbolic' : null;
                this.emit('text-changed', this.text);
            }
            this._entry_changed_by_widget = false;
        }));
        this.connect('enter-notify-event', this._on_motion.bind(this));
        this.connect('motion-notify-event', this._on_motion.bind(this));
        this.connect('leave-notify-event', this._on_leave.bind(this));

        this.get_style_context().add_class('endless-search-box');
    },

    _on_motion: function (widget, event) {
        let [has_coords, x, y] = event.get_root_coords();
        if (!has_coords)
            return;
        let rect = this.get_icon_area(Gtk.EntryIconPosition.PRIMARY);
        let top = this.get_toplevel();
        if (!top.is_toplevel())
            return;
        let [realized, icon_x, icon_y] = this.translate_coordinates(top,
            rect.x, rect.y);
        if (!realized)
            return;

        if (x >= icon_x && x <= icon_x + rect.width &&
            y >= icon_y && y <= icon_y + rect.height) {
            if (this._has_hand_cursor)
                return;
            let cursor = Gdk.Cursor.new_for_display(Gdk.Display.get_default(),
                Gdk.CursorType.HAND1);
            this.window.set_cursor(cursor);
            this._has_hand_cursor = true;
        } else {
            this._on_leave(widget);
        }
    },

    _on_leave: function (widget) {
        if (!this._has_hand_cursor)
            return;
        this.window.set_cursor(null);
        this._has_hand_cursor = false;
    },

    _onMatchSelected: function (widget, model, iter) {
        let index = model.get_path(iter).get_indices();
        this.emit('menu-item-selected', this._items[index]['id']);
    },

    /* Set the menu items by providing an array of item objects:
        [
            {
                'title': 'Frango',
                'id': 'http://www.myfrango.com'
            }
        ]

        'title' must be a string but 'id' can be any type and is used to
        identify the data that was selected.
    */
    set_menu_items: function (items) {
        this._items = items;
        let model = this._auto_complete.get_model();
        model.clear();
        for (let i = 0; i < this._items.length; i++) {
            model.set(model.append(), [0], [this._items[i]['title']]);
        }
        this._entry_changed_by_widget = true;
        this.emit('changed');
    }
});
