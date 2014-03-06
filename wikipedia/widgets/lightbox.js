const Lang = imports.lang;
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;

const Lightbox = new Lang.Class({
    Name: 'Lightbox',
    Extends: Gtk.Overlay,

    _init: function (params) {
        params = params || {};
        this.parent(params);

        // Not sure if there's a better way to do this, but this inner event
        // box is here only to block mouse events to the outer box. Even if
        // the lightbox widget does not intercept mouse event, this eventbox
        // will stop the outerbox from receiving mouse release event that
        // originated on the lightbox widget
        this._inner_eventbox = new Gtk.EventBox({
            // TODODOR
            // halign: Gtk.Align.CENTER,
            // valign: Gtk.Align.CENTER,
            margin: 100
        });
        this._inner_eventbox.show();
        this._inner_eventbox.connect("button-release-event",
                                     function (w, e) { return true; });

        this._shadow_eventbox = new Gtk.EventBox({
            halign: Gtk.Align.FILL,
            valign: Gtk.Align.FILL,
            no_show_all: true
        });
        this._shadow_eventbox.get_style_context().add_class("lightbox-shadow");
        this._shadow_eventbox.add(this._inner_eventbox);
        this._shadow_eventbox.connect("button-release-event",
                                      Lang.bind(this, this._button_event));
        this.show_lightbox();
 
        this.add_overlay(this._shadow_eventbox);
        this._widget = null;
    },

    _button_event: function (widget, event) {
        // button 1 is the left mouse click
        if (event.get_button()[1] === 1)
            this.hide_lightbox();
    },

    hide_lightbox: function () {
        this._shadow_eventbox.get_style_context().remove_class("lightbox-shadow-active");
        this._shadow_eventbox.hide();
    },

    show_lightbox: function () {
        this._shadow_eventbox.get_style_context().add_class("lightbox-shadow-active");
        this._shadow_eventbox.show();
    },

    add_lightbox_widget: function (widget) {
        if (this._widget !== null) {
            printerr("Already something in the lightbox!");
            return;
        }
        this._widget = widget;
        this._inner_eventbox.add(this._widget);
    },

    remove_lightbox_widget: function () {
        if (this._widget === null) {
            printerr("Nothing to remove from lightbox!");
            return;
        }
        this._inner_eventbox.add(this._widget);
        this._widget = null;
    }
});
