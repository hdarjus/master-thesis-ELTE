library(tidyverse)

tb_pietrzak <- tibble(lambda = integer(),
                      t = integer(),
                      lambdaRSW = integer(),
                      dmax = integer(),
                      keysize = integer(),
                      Prover.allocation = integer(),
                      Prover.y = integer(),
                      Prover.proof = integer(),
                      Prover.hash = integer(),
                      Verifier.allocation = integer(),
                      Verifier.proof = integer(),
                      Verifier.hash = integer())

con <- file("results_pietrzak.txt", open = "rt")
readLines(con, 1)

while (TRUE) {
  reslines <- try(readLines(con, 22, ok = FALSE), silent = TRUE)
  if (inherits(reslines, "try-error")) {
    break
  }
  tb_pietrzak <- tb_pietrzak %>%
    add_row(lambda = as.integer(strsplit(reslines[3], ":")[[1]][2]),
            t = as.integer(strsplit(reslines[4], ":")[[1]][2]),
            lambdaRSW = as.integer(strsplit(reslines[5], ":")[[1]][2]),
            dmax = as.integer(strsplit(reslines[6], ":")[[1]][2]),
            keysize = as.integer(strsplit(reslines[7], ":")[[1]][2]),
            Prover.allocation = as.integer(strsplit(reslines[12], ":")[[1]][2]),
            Prover.y = as.integer(strsplit(reslines[13], ":")[[1]][2]),
            Prover.proof = as.integer(strsplit(reslines[14], ":")[[1]][2]),
            Prover.hash = as.integer(strsplit(reslines[15], ":")[[1]][2]),
            Verifier.allocation = as.integer(strsplit(reslines[17], ":")[[1]][2]),
            Verifier.proof = as.integer(strsplit(reslines[18], ":")[[1]][2]),
            Verifier.hash = as.integer(strsplit(reslines[19], ":")[[1]][2]))
}

close(con)
