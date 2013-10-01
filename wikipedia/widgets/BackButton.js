const Endless = imports.gi.Endless;
const Gettext = imports.gettext;
const GLib = imports.gi.GLib;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const Config = imports.wikipedia.config;

const _ = function(string) { return GLib.dgettext('eos-sdk', string); };
Gettext.bindtextdomain('eos-sdk', Config.DATADIR + '/locale');

const BACK_BUTTON_URI = "resource://com/endlessm/wikipedia-domain/assets/topbar_back_icon_normal.png";

const BackButton = new Lang.Class({
    Name: 'BackButton',
    Extends: Endless.AssetButton,

    _init: function(props) {
        props = props || {};
        props.label = _("BACK");
        props.normal_image_uri = BACK_BUTTON_URI;

        this.parent(props);
    }
});
