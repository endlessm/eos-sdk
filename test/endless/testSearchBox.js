// Copyright (C) 2016 Endless Mobile, Inc.

const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;

const Utils = imports.test.utils;

Gtk.init(null);

describe('SearchBox', function () {
    let search_box;

    beforeEach(function () {
        search_box = new Endless.SearchBox();
    });

    it('emits no signal when you change the text programmatically', function () {
        search_box.connect('text-changed', () => fail());
        search_box.set_text_programmatically('some text');
        Utils.update_gui();
    });
});
