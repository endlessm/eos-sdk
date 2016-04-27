// Copyright (C) 2016 Endless Mobile, Inc.

const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;

Gtk.init(null);

describe('TopbarHomeButton', function () {
    let button;

    beforeEach(function () {
        button = new Endless.TopbarHomeButton();
    });

    it('can be constructed', function () {});
});
