grunt.loadNpmTasks('grunt-bower-concat');

bower_concat:{
        all: {
            dest: "src/js/vendor/bower.js",
            destCss: "src/css/vendor/bower.css"
        }
},
watch: {
        less: {
                files: [ 'bower.json' ],
                tasks: [ 'exec:bower_install' ]
        },
},
exec: {
        bower_install: {
                cmd: "bower install"
        }
}
