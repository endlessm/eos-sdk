const EndlessWikipedia = imports.wikipedia.EndlessWikipedia;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const TextButton = imports.textButton;

const HOVER_ARROW_URI = "/com/endlessm/wikipedia-domain/assets/submenu_hover_arrow.png";

const ArticleList = new Lang.Class({
    Name: 'ArticleList',
    Extends: Gtk.ScrolledWindow,
    
    Signals: {
        'article-chosen': {
            param_types: [GObject.TYPE_STRING, GObject.TYPE_INT]
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

        //width is set per designs, height is set arbitrarily for now but doesn't matter because it's just a min size
        this.set_size_request(258, -1);
        this.add(this._grid);
    },

    // Takes a list of dictionaries with keys 'title' and 'uri'
    setArticles: function(articles) {
        // Remove all existing article links
        this._grid.get_children().forEach(function(element, index, obj) {
            this._grid.remove(element);
        }, this);

        // Create new ones
        articles.forEach(function(title, index, obj) {
            let button = new TextButton.TextButton(HOVER_ARROW_URI, title, {hexpand:true});
            button.connect('clicked', Lang.bind(this, function() {
                this.emit('article-chosen', title, index);
            }));
            
            this._grid.add(button);
        }, this);
    }
});
