const Gtk = imports.gi.Gtk;

function update_gui () {
    while (Gtk.events_pending())
        Gtk.main_iteration(false);
}
