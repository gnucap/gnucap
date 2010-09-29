
done:
.admittance ycol (col icol) gcpr
            a[col][col] += (gcpr);
            a[icol][icol] += (gcpr);
            a[col][icol] += (-gcpr);
            a[icol][col] += (-gcpr);

.admittance yemit (emit iemit) gepr
            a[emit][emit] += (gepr);
            a[iemit][iemit] += (gepr);
            a[emit][iemit] += (-gepr);
            a[iemit][emit] += (-gepr);


.polyg0 yce (icol iemit) go
            a[icol][icol] += (go);
            a[iemit][iemit] += (go);
            a[icol][iemit] += (-go);
            a[iemit][icol] += (-go);


.polyg1 gm (icol iemit ibase iemit) gm
            a[iemit][iemit] += (gm);
            a[icol][ibase] += (gm);
            a[icol][iemit] += (-gm);
            a[iemit][ibase] += (-gm);

maybe:
.admittance ybase (base ibase) gx
            a[base][base] += (gx);
            a[ibase][ibase] += (gx);
            a[base][ibase] += (-gx);
            a[ibase][base] += (-gx);

.admittance ybc (ibase icol) gmu
            a[icol][icol] += (gmu);
            a[ibase][ibase] += (gmu);
            a[icol][ibase] += (-gmu);
            a[ibase][icol] += (-gmu);

.admittance ybe (ibase iemit) gpi
            a[ibase][ibase] += (gpi);
            a[iemit][iemit] += (gpi);
            a[ibase][iemit] += (-gpi);
            a[iemit][ibase] += (-gpi);



not done:
capacitance
.admittance ycs (icol subst) gccs
            a[icol][icol] += (gccs);
            a[subst][subst] += (gccs);
            a[icol][subst] += (-gccs);
            a[subst][icol] += (-gccs);

capacitance
.admittance ybcx (base icol) geqbx
            a[base][base] += (geqbx);
            a[icol][icol] += (geqbx);
            a[base][icol] += (-geqbx);
            a[icol][base] += (-geqbx);

transcap
.vccs gcb (iemit ibase icol ibase) geqcb
            a[ibase][ibase] += (geqcb);
            a[iemit][icol] += (geqcb);
            a[ibase][icol] += (-geqcb);
            a[iemit][ibase] += (-geqcb);



 
ceqcs=model->BJTtype * (*(ckt->CKTstate0 + here->BJTcqcs) - vcs * gccs);
b[icol] += (ceqcs);
b[subst] += (-ceqcs);

ceqbx=model->BJTtype * (*(ckt->CKTstate0 + here->BJTcqbx) - vbx * geqbx);
b[base] += (-ceqbx);
b[icol] += (ceqbx);

ceqbe=model->BJTtype * (cc + cb - vbe * (gm + go + gpi) + vbc * (go - geqcb));
ceqbc=model->BJTtype * (-cc + vbe * (gm + go) - vbc * (gmu + go));



b[ibase] += -(cb);
b[iemit] += (cb);

b[ibase] += (vbe*(gpi));
b[iemit] += (-vbe*(gpi));

b[ibase] += (vbc*(geqcb));
b[iemit] += -(vbc*(geqcb));



b[iemit] += (cc);
b[icol] += (-cc);

b[iemit] += (-vbe*(gm));
b[icol] += (vbe*(gm));

b[iemit] += (-vce*(go));
b[icol] += ((vce)*(go));



b[icol] += (-vbc*(gmu));
b[ibase] += (vbc*(gmu));

