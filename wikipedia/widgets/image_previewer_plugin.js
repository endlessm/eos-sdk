const Lang = imports.lang;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;
const GdkPixbuf = imports.gi.GdkPixbuf;
const GLib = imports.gi.GLib;

const ScalableImage = Lang.Class({
  Name: 'ScalableImage',
  Extends: Gtk.Widget,

  _init: function(params){
    params = params || {};
    params["app-paintable"] = true;
    this.parent(params);
    this.set_has_window(false);

    this._pixbuf = null;
    this._aspect = 1.0;
    this._file = null;
    this._last_file = null;
    this._last_allocation = null;
  },

  set_file: function (file) {
    this._file = file;
    let pixbuf = GdkPixbuf.Pixbuf.new_from_file(this._file);
    this._natural_width = pixbuf.get_width();
    this._aspect = pixbuf.get_width() / pixbuf.get_height();
    this.queue_draw();
  },

  _update_pixbuf: function () {
    let allocation = this.get_allocation();
    if(this._file === this._last_file && allocation === this._last_allocation)
      return;
    this._last_file = this._resource;
    this._last_allocation = allocation;
    this._pixbuf = GdkPixbuf.Pixbuf.new_from_file_at_scale(this._file,
                                                           allocation.width,
                                                           -1,
                                                           true);
  },

  vfunc_get_request_mode: function () {
    return Gtk.SizeRequestMode.HEIGHT_FOR_WIDTH;
  },

  vfunc_get_preferred_width: function () {
    return [25, this._natural_width];
  },

  vfunc_get_preferred_height_for_width: function (width) {
    let height = width / this._aspect;
    return [height, height];
  },

  vfunc_draw: function (cr) {
    this._update_pixbuf();
    if (this._pixbuf !== null) {
      Gdk.cairo_set_source_pixbuf(cr, this._pixbuf, 0, 0);
      cr.paint();
    }
    // We need to manually call dispose on cairo contexts. This is somewhat related to the bug listed here
    // https://bugzilla.gnome.org/show_bug.cgi?id=685513 for the shell. We should see if they come up with
    // a better fix in the future, i.e. fix this through gjs.
    cr.$dispose();
    return true;
  }
});

const ImagePreviewerPlugin = {
    init: function () {
        let formats = GdkPixbuf.Pixbuf.get_formats();
        this._supported_types = formats.reduce(function(type_list, format) {
            return type_list.concat(format.get_mime_types());
        }, []);
    },

    supports_type: function (type) {
        return this._supported_types.indexOf(type) != -1;
    },

    get_widget: function (file) {
        let frame = new ScalableImage();
        frame.set_file(file.get_path());
        return frame;
    }
};
