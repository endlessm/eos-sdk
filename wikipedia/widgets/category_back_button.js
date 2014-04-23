// Copyright 2014 Endless Mobile, Inc.

const Gettext = imports.gettext;
const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const CompositeButton = imports.wikipedia.widgets.composite_button;
const Config = imports.wikipedia.config;

const _ = function (string) { return GLib.dgettext('eos-sdk', string); };
Gettext.bindtextdomain('eos-sdk', Config.DATADIR + '/locale');

const CATEGORY_BACK_BUTTON_RESOURCE_URI = 'resource:///com/endlessm/wikipedia-domain/assets/wikipedia-category-back-symbolic.svg';
const CATEGORY_BACK_BUTTON_SIZE_PIXELS = 68;
const STYLE_CONTEXT_LABEL = 'label';
const STYLE_CONTEXT_BACK = 'back';

const CategoryBackButton = new Lang.Class({
    Name: 'CategoryBackButton',
    GTypeName: 'CategoryBackButton',
    Extends: CompositeButton.CompositeButton,

    _init: function(props) {
        this.parent(props);

        let gicon = new Gio.FileIcon({
            file: Gio.File.new_for_uri(CATEGORY_BACK_BUTTON_RESOURCE_URI)
        });
        let icon = Gtk.Image.new_from_gicon(gicon, Gtk.IconSize.DIALOG);
        icon.pixel_size = CATEGORY_BACK_BUTTON_SIZE_PIXELS;
        let label = new Gtk.Label({
            label: _("OTHER CATEGORIES")
        });
        let innerGrid = new Gtk.Grid({
            expand: true,
            valign: Gtk.Align.CENTER
        });

        innerGrid.add(icon);
        innerGrid.add(label);
        this.add(innerGrid);
        this.setSensitiveChildren([icon, label]);

        // Define style classes for CSS
        icon.get_style_context().add_class(Gtk.STYLE_CLASS_IMAGE);
        label.get_style_context().add_class(STYLE_CONTEXT_LABEL);
        this.get_style_context().add_class(STYLE_CONTEXT_BACK);
    }
});
