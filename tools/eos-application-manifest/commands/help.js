// Copyright 2013 Endless Mobile, Inc.

const System = imports.system;

function execute(args) {
    if (args.length === 0) {
        print('Usage: %s <command> [<args>]\n'.format(System.programInvocationName));
        // Query all available subcommands
        let oldSearchPath = imports.searchPath;
        imports.searchPath = [commandSearchPath]; // only pick up subcommands
        let commandsList = [];
        for (let commandName in imports) {
            commandsList.push(commandName);
        }
        imports.searchPath = oldSearchPath;

        // Print out summary for each subcommand
        if (commandsList.length === 0)
            return;
        print('Summaries of commands:');
        let maxWidth = commandsList.reduce(function (prev, curr) {
            return Math.max(curr.length, prev.length);
        });
        commandsList.forEach(function (commandName) {
            let command = imports[commandName];
            let summary;
            if (typeof command.summary == 'undefined')
                summary = 'No information available';
            else
                summary = command.summary();
            print('  %%%ds - %%s'.format(maxWidth).format(commandName, summary));
        });
        return;
    }

    const command = imports[args[0]];
    command.help();
}

function summary() {
    return 'Displays help information about a subcommand';
}

function help() {
    print("Displays help information about a subcommand.\n\
Try '%s help <command-name>'.".format(System.programInvocationName));
}
