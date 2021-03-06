const Gdk = imports.gi.Gdk;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

/**
 * Class: TopbarHomeButton
 * Generic button widget to re-direct the user to the home page
 *
 * This is a generic button widget to be used by all applications.
 * It must be placed at the top left corner of the window. When
 * clicked, the user must be re-directed to the home page of the
 * application.
 */
var TopbarHomeButton = new Lang.Class({
    Name: 'TopbarHomeButton',
    GTypeName: 'EosTopbarHomeButton',
    Extends: Gtk.Button,

    _init: function(props={}) {
        this.parent(props);

        let icon_name = 'go-home-symbolic';
        let image = Gtk.Image.new_from_icon_name(icon_name, Gtk.IconSize.SMALL_TOOLBAR);
        this.set_image(image);

        this.get_style_context().add_class('home');
        this.get_style_context().add_class(Gtk.STYLE_CLASS_LINKED);

        this.can_focus = false;
    },
});
