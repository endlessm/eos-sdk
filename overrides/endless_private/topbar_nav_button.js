const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const TopbarNavButton = new Lang.Class({
    Name: 'TopbarNavButton',
    GTypeName: 'EosTopbarNavButton',
    Extends: Gtk.Grid,
    Properties: {
        'back-button': GObject.ParamSpec.object('back-button', 'Back button',
            'Topbar back button widget',
            GObject.ParamFlags.READABLE,
            Gtk.Button.$gtype),
        'forward-button': GObject.ParamSpec.object('forward-button',
            'Forward button', 'Topbar forward button widget',
            GObject.ParamFlags.READABLE,
            Gtk.Button.$gtype)
    },

    _init: function(props) {
        props = props || {};
        props.orientation = Gtk.Orientation.HORIZONTAL;
        this.parent(props);

        this._back_button = Gtk.Button.new_from_icon_name('topbar-go-previous-symbolic',
            Gtk.IconSize.SMALL_TOOLBAR);
        this._forward_button = Gtk.Button.new_from_icon_name('topbar-go-next-symbolic',
            Gtk.IconSize.SMALL_TOOLBAR);
        this._back_button.can_focus = false;
        this._forward_button.can_focus = false;
        this._back_button.get_style_context().add_class('back');
        this._back_button.get_style_context().add_class(Gtk.STYLE_CLASS_LINKED);
        this._forward_button.get_style_context().add_class('forward');
        this._forward_button.get_style_context().add_class(Gtk.STYLE_CLASS_LINKED);

        this.add(this._back_button);
        this.add(this._forward_button);
    },

    get back_button() {
        return this._back_button;
    },

    get forward_button() {
        return this._forward_button;
    }
});
