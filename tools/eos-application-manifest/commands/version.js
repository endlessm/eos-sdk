// Copyright (C) 2013-2016 Endless Mobile, Inc.

const System = imports.system;

function execute(args) {
    print("%s version %s".format(System.programInvocationName, programVersion));
}

function summary() {
    return "Version information";
}

function help() {
    print("Prints the version and exits.");
}
