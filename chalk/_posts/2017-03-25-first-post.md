---
layout: post
title: "First post"
description: "Welcome to the blag!"
tags: [hello]
---

Hello! This website exists to document the creation of the JankPad, a portable game console built on TI's TM4C.

_Full disclosure: This project is for the Lab 10 Game Design Competition that's a part of the EE 319K course at UT Austin_

Over the next 5 weeks we hope to successfully design, implement, and ship the JankPad and post about our progress here. The primary purpose of this collection of posts is self-reference and documentation; mainly recording what we learn, design choices we make along with reasoning and such. These posts aren't really intended to be tutorials or educaional, but we're making them public on the off chance that they prove to be useful.

Anyways, here's an example of what a typical post might talk about.

## This Website!

Since this post is already pretty meta, let's talk about this website.

I was looking for something that's:
+ simple
+ structured for posts
+ static (no backend)
+ hostable w/Github Pages
+ simple

So, [Jekyll](https://jekyllrb.com/) was a pretty natural fit.

I ended up going with Nielson Ramon's [Chalk](https://github.com/nielsenramon/chalk) Jekyll Theme for similar reasons (simple and clean, etc.) As a result, this website's github is a little bit wonky: the gh-pages branch contains the flattened jekyll output while the usual jekyll files are in a different branch (blag).

Typically, github pages repos store only the jekyll copy of the website and allow github pages to build the website internally. Howeever, at the time of writing github pages only supports [a small set of themes](https://pages.github.com/themes/). I liked Chalk enough that the extra structural weirdness was worth it.

## The Repository

There isn't really much here right now. A few branches and mostly empty READMEs. The only detail worth mentioning is the license used: I opted to go with the MIT License over GPL/MPL for this repository. There isn't _really_ a practical difference since this entire repository is primarily for ourselves, but, again, on the off chance that someone finds this useful you'll be able to use it basically as you wish (no copyleft).

# Finally, some useful/relevant links:
+ <https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet> (I am a markdown noob)

_Rahul Butani_