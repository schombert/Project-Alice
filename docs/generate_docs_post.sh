# Replace broken code blocks in headers
sed -i "s/&lt;tt&gt;/<code>/g" out/html/md_*.html
sed -i "s/&lt;\/tt&gt;/<\/code>/g" out/html/md_*.html

# Set the about.md to be the start page
cp ./out/html/md_about.html ./out/html/index.html
