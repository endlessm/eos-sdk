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

        this._back_button = Gtk.Button.new_from_icon_name('topbar-go-previous-symbolic',
            Gtk.IconSize.SMALL_TOOLBAR);
        this._forward_button = Gtk.Button.new_from_icon_name('topbar-go-next-symbolic',
            Gtk.IconSize.SMALL_TOOLBAR);

        this._back_button.get_style_context().add_class('back');
        this._forward_button.get_style_context().add_class('forward');

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
