This repo will serve as a container of the numerical simulations the group is performing with Pluto. 

How to generate PDFs from the Markdown+LaTeX `README` files in a setup directory:

1. Use `mdtex.sh` script or ...
2. `pandoc --verbose --from=markdown --output=<OUTPUT.PDF> <INPUT.PDF> --variable=geometry:"margin=0.5cm, paperheight=500pt, paperwidth=400pt" --highlight-style=espresso --pdf-engine=xelatex`