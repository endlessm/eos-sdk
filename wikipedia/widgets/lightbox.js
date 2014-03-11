const Lang = imports.lang;
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;
const GObject = imports.gi.GObject;

const Lightbox = new Lang.Class({
    Name: 'Lightbox',
    Extends: Gtk.Overlay,

    _init: function (params) {
        params = params || {};
        this.parent(params);

        this._shadow_eventbox = new Gtk.EventBox({
            halign: Gtk.Align.FILL,
            valign: Gtk.Align.FILL,
            no_show_all: true
        });
        this._shadow_eventbox.get_style_context().add_class("lightbox-shadow");
        this._shadow_eventbox.connect("button-release-event",
                                      Lang.bind(this, this._button_event));
        this.show_lightbox();
 
        this.add_overlay(this._shadow_eventbox);
        this._widget = null;
    },

    _button_event: function (widget, event) {
        // button 1 is the left mouse click
        if (event.get_button()[1] !== 1)
            return;
        // if the event was generated on our child widgets GdkWindows, don't
        // hide lightbox
        if (event.get_window() != this._shadow_eventbox.get_window())
            return;
        let event_coords = event.get_coords().slice(1);
        let child_alloc = this._widget.get_allocation();
        // if event is generated inside child widgets allocation return, not
        // all child widgets will have there own GdkWindows capturing mouse
        // events
        if (event_coords[0] >= child_alloc.x &&
            event_coords[1] >= child_alloc.y &&
            event_coords[0] <= child_alloc.x + child_alloc.width &&
            event_coords[1] <= child_alloc.y + child_alloc.height)
            return;
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
        this._shadow_eventbox.add(this._widget);
    },

    remove_lightbox_widget: function () {
        if (this._widget === null) {
            printerr("Nothing to remove from lightbox!");
            return;
        }
        this._widget = null;
        this._shadow_eventbox.remove(this._widget);
    }
});
