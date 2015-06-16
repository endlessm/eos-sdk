const GObject = imports.gi.GObject;

function bind_align_expand (src_widget, target_widget) {
    let bindings = [];
    let bind_flags = GObject.BindingFlags.SYNC_CREATE;
    bindings.push(src_widget.bind_property("hexpand", target_widget, "hexpand", bind_flags));
    bindings.push(src_widget.bind_property("vexpand", target_widget, "vexpand", bind_flags));
    bindings.push(src_widget.bind_property("halign", target_widget, "halign", bind_flags));
    bindings.push(src_widget.bind_property("valign", target_widget, "valign", bind_flags));
    return bindings;
}
