const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const ArticleList = new Lang.Class({
    Name: 'ArticleList',
    Extends: Gtk.ScrolledWindow,
    Signals: {
        'article-chosen': {
            param_types: [GObject.TYPE_STRING, GObject.TYPE_STRING]
        }
    },

    _init: function(props) {
        props = props || {};
        props.hscrollbar_policy = Gtk.PolicyType.NEVER;
        props.vscrollbar_policy = Gtk.PolicyType.AUTOMATIC,
        this.parent(props);

        this._grid = new Gtk.Grid({
            orientation: Gtk.Orientation.VERTICAL,
            vexpand: true
        });
        this.add(this._grid);
    },

    // Takes a list of dictionaries with keys 'title' and 'url'
    setArticles: function(articles) {
        // Remove all existing article links
        this._grid.get_children().forEach(function(element, index, obj) {
            this._grid.remove(element);
        }, this);

        // Create new ones
        articles.forEach(function(element, index, obj) {
            var button = Gtk.Button.new_with_label(element.title.toUpperCase());
            button.image = Gtk.Image.new_from_icon_name('go-next-symbolic',
                Gtk.IconSize.BUTTON);
            button.always_show_image = true; // Don't do this, see BackButton.js
            button.image_position = Gtk.PositionType.RIGHT;
            button.xalign = 0;

            button.connect('clicked', Lang.bind(this, function() {
                this.emit('article-chosen', element.title, element.url);
            }));
            button.show();
            this._grid.add(button);
        }, this);
    }
});
