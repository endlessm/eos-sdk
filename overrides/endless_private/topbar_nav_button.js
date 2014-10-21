const Gdk = imports.gi.Gdk;
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

        let back_button_image;
        let forward_button_image;
        let is_rtl = this.get_default_direction() === Gtk.TextDirection.RTL ? true : false;
        if (is_rtl) {
            back_button_image = 'topbar-go-previous-rtl-symbolic';
            forward_button_image = 'topbar-go-next-rtl-symbolic';
        } else {
            back_button_image = 'topbar-go-previous-symbolic';
            forward_button_image = 'topbar-go-next-symbolic';
        }

        this._back_button = Gtk.Button.new_from_icon_name(back_button_image,
            Gtk.IconSize.SMALL_TOOLBAR);
        this._forward_button = Gtk.Button.new_from_icon_name(forward_button_image,
            Gtk.IconSize.SMALL_TOOLBAR);

        this._back_button.get_style_context().add_class('back');
        this._forward_button.get_style_context().add_class('forward');

        if (is_rtl) {
            this._back_button.get_style_context().add_class('rtl');
            this._forward_button.get_style_context().add_class('rtl');
        }

        [this._back_button, this._forward_button].forEach(function (button) {
            button.can_focus = false;
            button.add_events(Gdk.EventMask.ENTER_NOTIFY_MASK |
                Gdk.EventMask.LEAVE_NOTIFY_MASK);
            button.connect('enter-notify-event', function (widget) {
                let cursor = Gdk.Cursor.new_for_display(Gdk.Display.get_default(),
                    Gdk.CursorType.HAND1);
                widget.window.set_cursor(cursor);
            });
            button.connect('leave-notify-event', function (widget) {
                widget.window.set_cursor(null);
            });
            button.get_style_context().add_class(Gtk.STYLE_CLASS_LINKED);
        });

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
