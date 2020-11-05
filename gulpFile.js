const fs = require('fs');
const gulp = require('gulp');
const htmlmin = require('gulp-htmlmin');
const cleancss = require('gulp-clean-css');
const terser = require('gulp-terser');
const gzip = require('gulp-gzip');
const del = require('del');
const inline = require('gulp-inline');

const dataFolder = 'src/static/';

gulp.task('clean', function(done) {
    del([ dataFolder + '*']);
    done();
    return true;
});

gulp.task('buildfs_inline', gulp.series('clean', done => {
    done();
    return gulp.src('src/html/*.html')
        .pipe(inline({
            base: 'html/',
            js: terser,
            css: cleancss
        }))
        .pipe(htmlmin({
            collapseWhitespace: true,
            removecomments: true,
            aside: true,
            minifyCSS: true,
            minifyJS: true
        }))
        .pipe(gzip())
        .pipe(gulp.dest(dataFolder));
}));

gulp.task('buildfs_embeded', gulp.series('buildfs_inline', async () => {

    var source = dataFolder + 'index.html.gz';
    var destination = dataFolder + 'index.html.gz.h';

    var wstream = fs.createWriteStream(destination);
    wstream.on('error', function (err) {
        console.log(err);
    });

    var data = fs.readFileSync(source);

    wstream.write('#define index_html_gz_len ' + data.length + '\n');
    wstream.write('const uint8_t index_html_gz[] PROGMEM = {')

    for (i=0; i<data.length; i++) {
        if (i % 1000 == 0) wstream.write("\n");
        wstream.write('0x' + ('00' + data[i].toString(16)).slice(-2));
        if (i<data.length-1) wstream.write(',');
    }

    wstream.write('\n};')
    wstream.end();

    del([source]);

}));

gulp.task('default', gulp.series('buildfs_embeded'));
