# TSDD
This is Tagged Sentential Decision Diagram package.


To reference this paper, please use:

```shell
@inproceedings{fang2019tagged,
  title={Tagged Sentential Decision Diagrams: Combining Standard and Zero-suppressed Compression and Trimming Rules},
  author={Fang, Liangda and Fang, Biqing and Wan, Hai and Zheng, Zeqi and Chang, Liang and Yu, Quan},
  booktitle={2019 IEEE/ACM International Conference on Computer-Aided Design (ICCAD)},
  pages={1--8},
  year={2019},
  organization={IEEE}
}
```

## Make & Run
```bash
make
./tsdd ../sample/c17.cnf
```

## Dependencies
[graphviz](http://www.graphviz.org/) - for drawing TSDDs in png format

[expect](http://manpages.ubuntu.com/manpages/trusty/man1/expect.1.html) - for timeout of a test
